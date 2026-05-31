from django.urls import path
from . import views

urlpatterns = [
    path('',        views.index,       name='index'),
    path('save/',   views.save_key,    name='save'),
    path('crypt/',  views.encrypt,     name='crypt'),
    path('decrypt/', views.decrypt,    name='descrypt'),
]