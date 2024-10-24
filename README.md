# FIAP - Faculdade de Informática e Administração Paulista
<br/>
<br/>
<p align="center">
<a href= "https://www.fiap.com.br/"><img src="public/imagens/logo-fiap.png" alt="FIAP - Faculdade de Informática e Admnistração Paulista" border="0" width=40% height=40%></a>
</p>

<br>

# # 🌱 S.I.M.A - Sistema inteligente de monitoramento agrícola (ESP32)

## Grupo 15

## 👨‍🎓 Integrantes: 
- <a href="https://www.linkedin.com/in/a1exlima/">Alex da Silva Lima</a>
- <a href="https://www.linkedin.com/in/johnatanloriano/">Johnatan Sousa Macedo Loriano</a>
- <a href="https://www.linkedin.com/in/matheus-maia-655bb1250/">Matheus Augusto Rodrigues Maia</a>

## 👩‍🏫 Professores:
### Tutor(a) 
- <a href="https://www.linkedin.com/in/lucas-gomes-moreira-15a8452a/">Lucas Gomes Moreira</a>
### Coordenador(a)
- <a href="https://www.linkedin.com/in/profandregodoi/">André Godoi</a>


## 📜 Descrição

#### Introdução

## 📁 Estrutura de pastas

Dentre os arquivos e pastas presentes na raiz do projeto, definem-se:

- <b>data</b>: Esta pasta contém o arquivo <i>lista_produtos_agro.json</i>, que armazena dados de categorias e produtos pré-cadastrados utilizados para o registro de transporte. Esses dados são essenciais para a correta operação e monitoramento do sistema.

- <b>models</b>: Contém os scripts Python (<i>.py</i>) responsáveis pelos subalgoritmos, procedimentos e funções que suportam a aplicação principal do projeto. 

- <b>public</b>: Armazena todos os arquivos públicos utilizados para a explicação e desenvolvimento do projeto, como imagens e materiais de referência. 

- <b>scripts</b>: Contém scripts auxiliares para tarefas específicas do projeto, como migração de banco de dados e diagramas de modelagem entidade-relacionamento.

- <b>.env.example</b>: Um arquivo de configuração de exemplo que serve como template para a inserção de dados sensíveis do projeto. Este arquivo é crucial para configurar o ambiente 
de execução de maneira segura.

- <b>.gitignore</b>: Arquivo de configuração do Git que especifica quais arquivos e pastas devem ser ignorados pelo controle de versão.

- <b>app.py</b>: O arquivo principal do projeto, onde está implementado o código base da aplicação em Python.

- <b>log_monitoramento_transporte.txt</b>: Arquivo de texto que registra todos os logs gerados pelo simulador de sensor de temperatura. (É gerado após o usuário alterar status para andamento.)

- <b>README.md</b>: Arquivo de documentação em formato Markdown que serve como guia e explicação geral sobre o projeto.

- <b>requirements.txt</b>: Lista todas as bibliotecas e dependências Python necessárias para a inicialização e funcionamento do projeto.


## 🔧 Como executar o código

### 📝 Fase 1 - Copiando a aplicação

Clone o projeto para o local desejado em seu computador.

```bash
git clone https://github.com/fiap-small-farmer/atm-agricultural-transport-monitoring.git
```
#
### 🛢 Fase 2 - Preparando o banco de dados

- Usaremos o Oracle Database XE em conjunto com a ferramenta de desenvolvimento Oracle SQL Developer 23.1.1 para criação do banco de dados, caso NÃO tenha instalado ainda em sua máquina, acesse os links abaixo e execute a instalação.

- <a href="https://www.oracle.com/br/database/technologies/xe-downloads.html">Oracle Database XE: Versão gratuita e leve do Oracle Database, ideal para desenvolvimento, implantação e distribuição.</a>
- <a href="https://www.oracle.com/database/sqldeveloper/technologies/download/">SQL Developer 23.1.1: Ferramenta de desenvolvimento que facilita a conexão, consulta, administração e manipulação de dados de forma visual e interativa.</a>
- Após a instalação, acesse o Oracle SQL Developer, no canto superior esquerdo, clique no sinal de + em verde e configure uma nova conexão, seguindo o exemplo abaixo:
  
<br>
<p align="center">
<img src="public/imagens/config_conexao_db.png" alt="FIAP - Faculdade de Informática e Admnistração Paulista" border="0" width=60% height=60%>
</p>

- Siga os passos:

1. Informe o nome da conexão.
2. Insira o nome de usuário e senha.
3. Insira os detalhes de conexão, como nome do host, porta e SID.
4. Clique em testar, se o status apresentar: "Com Sucesso", banco de dados está conectado com o servidor.

- Acesse no projeto a pasta scripts e abra o arquivo (script_v1.sql) dentro do Oracle SQL Developer.
- Execute as instruções clicando em F5, após finalizar, verifique se o status de saída apresenta:

```PowerShell
    Table PRODUTO criado.

    Table ORIGEM criado.

    Table DESTINO criado.

    Table TRANSPORTE criado.
```

- Acesse o arquivo .env.example dentro do projeto e renomeie para .env
- Insira dentro do arquivo .env seguindo o exemplo abaixo, o mesmo nome de usuário, senha e dados relacionados ao DNS configurados no banco de dados.

```PowerShell
    USER=rmxxxxxx
    PASSWORD=DDMMAA
    DNS=oracle.fiap.com.br:1521/ORCL
```
#
### 👨🏻‍💻 Fase 3 - Preparando a aplicação para execução

Entre na pasta raiz do repositório

```PowerShell
#  Navega para o diretório do projeto no terminal.

cd atm-agricultural-transport-monitoring
```

Execute os comandos:

```PowerShell
# Cria um ambiente virtual isolado para projetos Python, gerencia dependências separadamente.

py -m venv venv
```

```PowerShell
# Ativa o ambiente virtual Python no Windows, permitindo usar suas dependências isoladas.

.\venv\Scripts\activate
```

```PowerShell
# Instala todas as dependências listadas no arquivo requirements.txt

pip install -r requirements.txt
```

Execute a aplicação localmente:

```PowerShell
# Executa o arquivo app.py usando o interpretador Python.

python app.py
```


## 🗃 Histórico de lançamentos

* 1.0.0 - 11/10/2024
#
## 📋 Licença

<img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1"><p xmlns:cc="http://creativecommons.org/ns#" xmlns:dct="http://purl.org/dc/terms/"><a property="dct:title" rel="cc:attributionURL" href="https://github.com/agodoi/template">MODELO GIT FIAP</a> por <a rel="cc:attributionURL dct:creator" property="cc:attributionName" href="https://fiap.com.br">Fiap</a> está licenciado sobre <a href="http://creativecommons.org/licenses/by/4.0/?ref=chooser-v1" target="_blank" rel="license noopener noreferrer" style="display:inline-block;">Attribution 4.0 International</a>.