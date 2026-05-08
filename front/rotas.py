from main import app
from flask import render_template

@app.route("/")
def paginainicial():
    return render_template("homepage.html")

@app.route("/sobre")
def sobre(): 
    return 'pagina sobre os integrantes do projeto'

@app.route("/criptografia") 
def cripto():
    return 'pagina criptografia'

@app.route("/") 
def assunto():
    return 'pagina para dizer sobre o assunto do projeto'
