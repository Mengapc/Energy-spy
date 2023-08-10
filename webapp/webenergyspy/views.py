from django.shortcuts import render

# Create your views here.
from dsjapi.decorators import api
from dsjapi.fields import *

from django.views.decorators.csrf import csrf_exempt

from .models import *

# Home
def home(request):
   if request.user.is_authenticated:
      cliente = Cliente.objects.filter(user = request.user).first()
      if cliente:
         try:
            obj = Leitura.objects.filter(cliente = cliente).latest('id')
            if obj:
               return render(request, 'home.html', {'ult_leitura': obj, 'cliente': cliente})
            else:
               return render(request, 'home.html', {'sem_leitura': '1', 'cliente': cliente})   
         except:
            return render(request, 'home.html', {'sem_leitura': '1', 'cliente': cliente})   
      else:
         return render(request, 'home.html', {'nao_cliente': '1'})
   else:
      return render(request, 'home.html', {})


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

