<?php

error_reporting(-1);
ini_set('display_errors', 'On');
ini_set('display_startup_errors', 1);

function get()
{
    // Validate ID
    if( !isset($_GET['id']) ) {
        return;
    }
    $id = $_GET['id'];
    if ( !preg_match('/^[A-Fa-f0-9]*$/', $id) || strlen($id) != 6 ) {
        return;
    }

    // Validate TOKEN
    if( !isset($_GET['token']) ) {
        return;
    }
    $token = $_GET['token'];
    if ( !preg_match('/^[A-Fa-f0-9_-]*$/', $token) || strlen($token) != 14 ) {
        return;
    }

    // use Cassandra; // apache don't seem to care for this
    $cluster   = Cassandra::cluster()->build();
    $keyspace  = 'dados';
    $session   = $cluster->connect($keyspace);
    // query sala
    $query = "SELECT sala FROM dados.salas_por_nodeid WHERE id='" . $id . "' and access_token='" . $token . "';";

    $result = $session->execute(new Cassandra\SimpleStatement($query));
    if (!$result) {
        return;
    }
    // return first result
    $row = $result->first();
    if (!$row) {
        return;
    }
    $sala = $row['sala'];

    // query uid
    $query = "SELECT * FROM dados.uid_por_salas WHERE sala='" . $sala . "';";

    $result = $session->execute(new Cassandra\SimpleStatement($query));

    foreach ($result as $row) {
        echo $row['uid'];
        echo "\r\n";
    }
}

function post()
{
    // TODO
    return;
}


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    post();
}
else if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    get();
}

?>
