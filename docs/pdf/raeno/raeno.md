# Relatório PARCIAL de Atividades de Estágio Não Obrigatório (RAENO) - TCE Nº 710322

## Coordenador de Estágios/Coordenador do Curso
Luiz Antonio Maccari Junior

## Semestre
Oitavo Semestre

## Atividades Desenvolvidas no Estágio
Durante o estágio, pretende-se integrar o sistema de controle de portas da UFSC Blumenau à plataforma IoT para controle de acesso dos laboratórios da UFSC. Esses sistema precisa ser automatizado, regularmente sincronizado com o banco de dados, seguro, e o mais importante, confiável.

Diversas abordagens foram pensadas para tal implementação. O sistema todo vai ser implementado na plataforma EPOS Mote III, desenvolvida na UFSC, pelo LISHA, assim como um dispositivo Wifi ESP8266 para acessar a rede eduroam e sincronizar tanto as tabelas de acesso, relógio interno e gerar logs de acesso. O EPOS é um sistema operacional desenhado com o propósito de atender às especificações de dispositivos através de Application-Driven Embedded System Design (ADESD), onde as aplicações são abstraídas e montadas peça a peça (denominadas 'traits', ou 'características') para se adaptar às aplicações dos usuários.

A primeira parte do projeto consistiu em se familiarizar com a plataforma de desenvolvimento EPOS. Como o paradigma de programação dele (alto nível e orientado à aplicação) é muito diferente do paradigma de microcontroladores convencionais (baixo nível e orientado ao hardware físico), foi inicialmente difícil se adaptar. O sistema de traits não era claro quanto ao modo de controle de hardware, como quais portas estavam disponíveis e quais já estavam sendo utilizadas pelas abstrações de software como dispositivos. Também havia o problema de grande parte da documentação se voltar diretamente para a teoria do software, mas não para a parte de hardware, deixando à cargo do programador procurar as respostas, como a declaração de portas, por exemplo, no código-fonte. Também foi importante se familiarizar com o estilo de codificação que o sistema usa para se manter ao mesmo tempo organizado e conciso, sem códigos que sejam redundantes e sendo o mais genérico possível. Muitos desses conceitos que não seriam aprendidos em uma situação de aula, simplesmente porque um sistema desenvolvido em aula não teria a complexidade de um sistema que já vem sendo desenvolvido e refinado ao longo dos anos.

A segunda parte foi projetar a parte que deveria rodar em nuvem. Ela é composta de duas partes principais: a parte de aquisição de dados, que serve para autenticar cada nodo sem fio e retornar a relação da tabela de acessos, para ser atualizada em cada controlador de portas em execução, e uma parte dedicada a logs, que armazenará cada acesso realizado com sucesso e cada acesso negado, bem como reportará problemas com os módulos. Um dos grandes desafios, e que ainda está em desenvolvimento, é estabelecer segurança e autenticidade na conexão módulo-servidor, para evitar ataques como interceptação de dados, onde dados podem ser comprometidos e alterados em trânsito. Atualmente, o sistema utiliza criptografia de ponta-a-ponta TLS e autenticação por chave pré-compartilhada, mas ela deve migrar para uma alternativa mais robusta em breve. O software que faz o gerenciamento e fornece os serviços de nuvem é o Apache versão 2, rodando o módulo PHP7.

Além disso, a parte em nuvem contém um módulo adicional de banco de dados para armazenar as informações dos nodos. Foi configurado um banco de dados NoSQL chamado Apache Cassandra que é altamente escalável e foi projetado para lidar com dados em esquema flexível, fornecendo alta performance de armazenamento e consulta de dados. Escolher um sistema adequado se faz necessário para não incorrer em problemas no futuro, caso se queira expandir a rede IoT de Blumenau para armazenar dados de outros tipos de sensores. Idealmente, esse serviço deveria oferecer uma API pública para dispositivos IoT no campus. O Cassandra se encaixa bem nesse paradigma pois permite o deployment de vários clusters em diferentes servidores, melhor distribuindo a carga.

Para integrar o EPOS com a nuvem, foi utilizado o módulo ESP8266. Para substituir o firmware padrão e se conectar à eduroam, foi escrito um firmware baseado no firmware original (disponibilizado pela espressif), que fornece uma interface AT via comunicação serial para se conectar e comunicar com módulos periférico e permitindo ao EPOS Mote III acessar a internet. Com isso em mente, o desafio é então desenvolver a aplicação ESP8266 dentro do ambiente EPOS. O objetivo de desenvolver essa aplicação é abstrair o hardware, o módulo físico, em um padrão denominado NIC (Network interface card, ou placa de interface de rede), que oferece funções comuns a todos os dispositivos que acessam algum meio de comunicação, como funções send(), receive() e rssi(), por exemplo. Essa interface foi desenvolvida sobre o padrão HTTP, suportando tanto HTTP (hyper text transfer protocol) quanto HTTPS (mesmo que HTTP, mas com criptografia dos dados), para permitir que o programador apenas precise enviar ou receber pacotes de dados através de requisições HTTP GET e HTTP POST. Pretende-se suportar também na mesma interface o protocolo MQTT, relativamente novo no ramo de IoT, até o fim do projeto.

Para leitura de cartões RFID, foi utilizado o módulo MFRC522, que é um leitor de tags RFIDs to tipo MIFARE de baixo custo. Sua interface já estava implementada no código base do EPOS, mas algum trabalho foi necessário para descobrir que portas deveriam ser conectadas ao módulo. Também foi adicionado parâmetros de debug para o módulo para diagnosticar problemas, que ainda precisam ser adicionadas ao repositório.


## Disciplinas Relacionadas com o Estágio
BLU3202 Algoritmos e Estruturas de Dados
BLU3302 Microprocessadores
BLU3403 Sistemas de Automação
BLU3505 Redes Industriais
BLU3503 Eletrônica Aplicada
BLU3020 Sistemas Computacionais para Controle e Automação
BLU3423 Programação Orientada a Objetos

## Importância do Estágio

Acredito que esse estágio esteja sendo muito importante para a formação principalmente porque o conceito de IoT é muito recente e ainda não se fez presente nas grades curriculares, mas que tem crescido nas indústrias.

Aqui também entra uma boa complementação acadêmica ao desenvolvimento de aplicações: nenhuma das matérias (Algoritmos e Estruturas de Dados ou Programação Orientada à objetos) prepara os alunos para a complexidade de um sistema real. Muitos dos conceitos tiveram que ser aprendidos por conta. A Universidade dá suporte com o básico das ferramentas para programação, mas não ensina nem parte do que foi utilizado para o desenvolvimento das aplicações, como templating, abstrações de software para que ele compile e funcione em em diversas plataformas, controle de versão (com ferramentas como git ou subversion), organização de código-fonte, etc. A legibilidade e documentação do código também é um fator muito importante quando se compartilha o código, para ter certeza que outras pessoas possam entender o que você produziu.

Pode-se dizer que o estágio contribuiu bastante para o aprendizado de C++, mas também em como a organização correta do código-fonte é importante para o desenvolvimento. Além disso, como o EPOS é multiplataforma, ele adiciona um elemento importante que ainda não foi estudado a fundo no curso, que é a virtualização do microcontrolador: é possível desenvolver a aplicação sem precisar do hardware físico porque o software pode ser todo virtualizado através do 'qemu-arm'. Esse conceito é muito útil para desenvolvimento de partes do projeto que não necessitam do hardware físico, acelerando esses processos.

Outro conceito que está sendo explorado durante o estágio é o desenvolvimento de aplicações em nuvem que sejam confiáveis e simples o suficiente para dispositivos com capacidade de processamento e memória limitadas, que podem ter dificuldade de processar um grande volume de informações ou mesmo lidar com criptografia.

## Relação com TCC/Monografia

Pretende-se utilizar esse trabalho realizado durante o estágio, junto com o que foi desenvolvido durante a pesquisa, no Trabalho de conclusão de curso, no entanto tudo vai depender do estágio obrigatório que será realizado no décimo semestre.
