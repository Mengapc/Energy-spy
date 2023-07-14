from django.db import models
from django.conf import settings

# Create your models here.

class Cliente(models.Model):
   cod_cliente = models.BigAutoField(primary_key=True)
   nome = models.CharField(max_length=100)
   email = models.CharField(max_length=100)
   endereco = models.CharField(max_length=255)
   user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.RESTRICT)

class Leitura(models.Model):
   datahora = models.DateTimeField(auto_now_add=True)
   tensao = models.FloatField()
   potencia = models.FloatField()
   corrente = models.FloatField()
   cliente = models.ForeignKey(Cliente, on_delete=models.RESTRICT)
   class Meta:
      constraints = [
         models.UniqueConstraint(fields=["datahora", "cliente"], name='leitura_ukey')
      ]
