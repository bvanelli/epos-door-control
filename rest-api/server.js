const restify = require('restify');
const errors = require('restify-errors');
const cassandra = require('cassandra-driver');
const fs = require('fs');

/*

    Setup Cassandra driver

*/
const client = new cassandra.Client({ contactPoints: ['127.0.0.1'], keyspace: 'dados' });

/*

    Regex constants

*/
const reghex = /^[0-9A-Fa-f]{6}$/;
const regb64 = /^[0-9A-Za-z_-]{14}$/;    
const regint = /^[-+]?\d{1,13}$/;

/*

    Configure HTTPS Server

*/
var https_options = {

    key: fs.readFileSync('./certs/server.key'),
    certificate: fs.readFileSync('./certs/server.crt')

};
var server = restify.createServer(https_options);
server.use(restify.plugins.acceptParser(server.acceptable));
server.use(restify.plugins.bodyParser());

/*

    Validation and authentication

*/
function validate(req) {
    return reghex.test(req.params.id) && regb64.test(req.params.token);
}
function validate_body(req) {
    return regint.test(req.body.uid) && regint.test(req.body.timestamp) && regint.test(req.body.code);
}

function authenticate(req, res, next, fn) {
    var sala = "";
    const query = 'SELECT sala FROM dados.salas_por_nodeid WHERE id=? AND access_token=?';
    const params = [ req.params.id, req.params.token ];
    client.execute(query, params, { prepare: true }, function (err, result) {
        if (err) {
            return next(new errors.InternalServerError());
        }
        if ( !(result.rows.length > 0) ) {
            return next(new errors.UnauthorizedError());
        }
        sala = result.first().sala;
        fn(sala);
    });
}

/*

    GET PATH

*/
server.get('/api/v1/:id/:token', (req, res, next) => {
    
    res.contentType = "text/plain";

    if (!validate(req)) {
        return next(new errors.BadRequestError());
    }
    
    authenticate(req, res, next, (sala) => {
        if (!sala) {
            return next(new errors.NotFoundError());
        }
        var ret = "";
        const query = 'SELECT uid FROM dados.uid_por_salas WHERE sala=?';
        const params = [ sala ];
        client.eachRow(query, params, { prepare: true },
            function (n, row) {
                ret += row.uid;
                ret += "\r\n";
            },
            function (err) {
                if (err) {
                    return next(new errors.InternalServerError());
                }
                res.send(ret);
                return next();
        });
    });
});

/*

    POST PATH

*/
server.post('/api/v1/:id/:token', (req, res, next) => {

    res.contentType = "text/plain";

    if (!validate(req)) {
        return next(new errors.BadRequestError());
    }
    
    authenticate(req, res, next, (sala) => {
        if (!sala) {
            return next(new errors.NotFoundError());
        }
        if ( !req.body || !req.body.uid || !req.body.timestamp || !req.body.code) {
            return next(new errors.BadRequestError());
        }
        if ( !validate_body(req) ) {
            return next(new errors.BadRequestError());
        }
        const uid = Number(req.body.uid);
        var timestamp = Number(req.body.timestamp);
        const code = Number(req.body.code);
        
        if (isNaN(uid) || isNaN(timestamp) || isNaN(code)) {
            return next(new errors.BadRequestError());
        }
        
        if (timestamp == 0) {
            timestamp = (new Date).getTime();
        }

        if (timestamp < 1445385600000) {
            return next(new errors.ImATeapotError('https://i.imgur.com/KQu2mXz.jpg'));
        }

        const query = 'INSERT INTO dados.log_acesso (uid, timestamp, status_code, id, sala) VALUES (?, ?, ?, ?, ?) IF NOT EXISTS';
        const params = [ uid, new Date(timestamp), code, req.params.id, sala];
        client.execute(query, params, { prepare: true }, function (err, result) {
            if (err) {
                return next(new errors.InternalServerError());
            }
            if ( !result.first()['[applied]']) {
                return next(new errors.ConflictError());
            }
            res.send('OK');
            return next();
        });
    });
});



/*

    Plugins and addons

*/
server.pre(restify.pre.dedupeSlashes());

server.on('uncaughtException', function(req, res, route, err) {
    console.log(err);
    return next(new errors.InternalServerError());
});

server.listen(8080, function() {

    console.log('%s listening at %s', server.name, server.url);

});
