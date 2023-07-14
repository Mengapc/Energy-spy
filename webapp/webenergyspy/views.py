from django.shortcuts import render

# Create your views here.
from dsjapi.decorators import api
from dsjapi.fields import *

from django.views.decorators.csrf import csrf_exempt

from .models import *

# Serviço a ser chamado pelo Arduino
@api (DictField ({
   "cod_cliente": IntField (min=1, max=999999),
   "tensao": FloatField(min=0),
   "potencia": FloatField(min=0),
   "corrente": FloatField(min=0)
}), allowedMethods=["POST"])

@csrf_exempt
def enviar_leitura(data):
   cod_cliente = data["cod_cliente"]
   try:
      cli = Cliente.objects.get(pk=cod_cliente)
   except:
      return {
         "resultado": "error",
         "mensagem": "cliente não existe"
      }
   leitura = Leitura()
   leitura.cliente = cli
   leitura.tensao = data["tensao"]
   leitura.potencia = data["potencia"]
   leitura.corrente = data["corrente"]
   try:
      leitura.save()
   except:
      return {
         "resultado": "error",
         "mensagem": "erro ao salvar leitura"
      }
   result = "ok"
   return {
      "result": result,
      "cod_cliente": leitura.cliente.cod_cliente,
      "nome_cliente": leitura.cliente.nome,
      "datahora": leitura.datahora
   }

