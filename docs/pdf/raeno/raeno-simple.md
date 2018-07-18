# Relatório PARCIAL de Atividades de Estágio Não Obrigatório (RAENO) - TCE Nº 710322

## Coordenador de Estágios/Coordenador do Curso
Luiz Antonio Maccari Junior

## Semestre
Oitavo Semestre

## Atividades Desenvolvidas no Estágio
Durante o estágio, pretende-se integrar o sistema de controle de portas da UFSC Blumenau à plataforma IoT para controle de acesso dos laboratórios da UFSC. Esses sistema precisa ser automatizado, regularmente sincronizado com o banco de dados, seguro, e o mais importante, confiável.

Diversas abordagens foram pensadas para tal implementação. O sistema todo vai ser implementado na plataforma EPOS Mote III, desenvolvida na UFSC, pelo LISHA, assim como um dispositivo Wifi ESP8266 para acessar a rede eduroam e sincronizar tanto as tabelas de acesso, relógio interno e gerar logs de acesso. O EPOS é um sistema operacional desenhado com o propósito de atender às especificações de dispositivos através de Application-Driven Embedded System Design (ADESD), onde as aplicações são abstraídas e montadas peça a peça (denominadas 'traits', ou 'características') para se adaptar às aplicações dos usuários.

A primeira parte do projeto consistiu em se familiarizar com a plataforma de desenvolvimento EPOS. Como o paradigma de programação dele (alto nível e orientado à aplicação) é muito diferente do paradigma de microcontroladores convencionais (baixo nível e orientado ao hardware físico), foi inicialmente difícil se adaptar.

A segunda parte foi projetar a parte que deveria rodar em nuvem. Ela é composta de duas partes principais: a parte de aquisição de dados, que serve para autenticar cada nodo sem fio e retornar a relação da tabela de acessos, para ser atualizada em cada controlador de portas em execução, e uma parte dedicada a logs, que armazenará cada acesso realizado com sucesso e cada acesso negado, bem como reportará problemas com os módulos. Um dos grandes desafios, e que ainda está em desenvolvimento, é estabelecer segurança e autenticidade na conexão módulo-servidor, para evitar ataques como interceptação de dados. O serviço em nuvem ainda conta com o banco de dados NoSQL Cassandra.

A parte que está em andamento é finalizar o protótipo e realizar testes.


## Disciplinas Relacionadas com o Estágio
BLU3202 Algoritmos e Estruturas de Dados
BLU3302 Microprocessadores
BLU3403 Sistemas de Automação
BLU3505 Redes Industriais
BLU3503 Eletrônica Aplicada
BLU3020 Sistemas Computacionais para Controle e Automação
BLU3423 Programação Orientada a Objetos

## Importância do Estágio

Acredito que esse estágio esteja sendo muito importante para a formação principalmente porque o conceito de IoT é muito recente e ainda não se fez presente nas grades curriculares, mas que tem crescido nas indústrias. Aqui também entra uma boa complementação acadêmica ao desenvolvimento de aplicações: nenhuma das matérias de programação prepara os alunos para a complexidade de um sistema real. Muitos dos conceitos tiveram que ser aprendidos por conta. A Universidade dá suporte com o básico das ferramentas para programação, mas não ensina nem parte do que foi utilizado para o desenvolvimento das aplicações, como templating, abstrações de software para que ele compile e funcione em em diversas plataformas, controle de versão (com ferramentas como git ou subversion), organização de código-fonte, etc. A legibilidade e documentação do código também é um fator muito importante quando se compartilha o código, para ter certeza que outras pessoas possam entender o que você produziu.

## Relação com TCC/Monografia

Pretende-se utilizar esse trabalho realizado durante o estágio, junto com o que foi desenvolvido durante a pesquisa, no Trabalho de conclusão de curso, no entanto tudo vai depender do estágio obrigatório que será realizado no décimo semestre.
