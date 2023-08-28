# Python Django
## Linux/Mac
```
python -m venv env
source env/bin/activate
pip install django
```
## Windows
```
python -m venv env_windows
./env_windows/Scripts/activate
pip install django
```

## Sequencia de criação de um novo projeto (Não deve ser executado novamente)

django-admin startproject energyspy .

django-admin startapp webenergyspy

## Instalação dos pacotes necessários
```
pip install psycopg2

pip install django-simple-json-api
pip install python-dateutil
```

# Modelo de banco de dados (Somente na criação de uma nova base de dados)
```
python manage.py makemigrations webenergyspy
python manage.py migrate
```

## Definindo super usuário
python manage.py createsuperuser
(menga - tom....1)

# Adicionar WebenergyspyConfig no arquivo energyspy/settings.py:
INSTALLED_APPS = [
    "webenergyspy.apps.WebenergyspyConfig",
    ...
]

# Executando o servidor
python manage.py runserver


# PostgreSQL Database
Host: energyspydb
Usuário: psql
Senha: ES061006!


# Deploy
az login

az webapp up --resource-group energy-spy -r "PYTHON|3.11" --location "Brazil South" --plan 5aa29ddd-be0c-4e4a-af31-658675bda958 --sku B1 --name energyspy

# Acessar servidor WEB
https://energyspy.azurewebsites.net

# Open SSH:
energyspy.scm.azurewebsites.net/webssh/host

