#ifndef _HTML_
#define _HTML_

const char index_html[] PROGMEM = R"rawliteral(


<!DOCTYPE html> 

<html lang="pt-br">

  <head>
    <meta charset="UTF-8" />
    <title>--Formulário Contato--</title>

    <script>
      function submitMessage(){
       alert("Deseja realmente alterar ?");
        setTimeout(function(){document.location.reload(false);},500);
      }
    </script>

    <style>

    body{
      width:100vw;
      
      min-height: 100vh;

      
    }

    .parent {
      display: flex;
      background-color: #ffffa0;
      width:100%%;
      
      max-width: 100%%;

      min-height: 100vh;
      background-size:contain;
        
    }
      
    .child {
      

      width:50vh;
      height:500px;

      background-color: #FFFF00;
      margin: auto;

      border-radius: 15px;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.8);
      width: 550px;
      max-width: 100;
    }

    form {
      font-family: Verdana,Tahoma,sans-serif;
      font-size: 15px;
      max-width: 600px;
    }

    @media screen and (orientation: portrait){

      .child {
      

        width:100vw;
        height:50vh;

        background-color: #FFFF00;
        margin: auto;

        border-radius: 15px;
        box-shadow: 0 4px 20px rgba(0, 0, 0, 0.8);
        
      }

      form {
        font-family: Verdana,Tahoma,sans-serif;
        font-size: 2rem;
        max-width: 100vw;
      }

    }





    * {
      margin: 0;
      box-sixing: border-box;
    }



    h1 {
      margin-top: 30px;
      margin-bottom: 20px;
      margin-left: 20px;
      color: #0b0c0c;
    }

    h3{
      margin-bottom: 20px;
      margin-left: 20px;
    }

    .salvar {
      height: auto;
      width: auto; 
             
    }

    input:hover, textarea:hover, input:focus, textarea:focus {
      border-color: #000066;
      box-shadow: 5px 0px 9px #FFFF00;
      background-color: #EDED00;
      color: black;
      font-weight: bold;
    }

    input, textarea {
      width: 200px;
      padding: 10px;
      color: black;
      margin-left: 20px;
      margin-top: 10px;
      -webkit-border-radius: 5px;
      -moz-border-radius: 5px;
      border: 1px solid;
    }

    form label {
      margin-left: 20px;
      color: #050404;
      font-weight: bold;
    }

    </style>
  
  </head>

  <body>





  <div class = "parent">

    <div class = "child">

      <form action="/get" target="hidden-form" autocomplete="off">

        <p>
            <h1>Melissa configuração da Caixa</h1>
        </p>
  
        <!--
            Expressão regex para só aceitar o id da caixa com ( MEL e 7 digitos numericos depois )
            ^[M][E][L][0-9]{7}$      
        -->
        <p>  
            <label for="i_caixa">
              <h3>ID DA CAIXA ATUAL: %ID_Caixa% </h3>
              
            </label>

             
            <input type="text" name="ID_Caixa" id="i_caixa" >
            <input type="submit" class="salvar" value="Alterar" onclick="submitMessage()">
        
        </p>
        <br>
    </form>



    <form action="/get" autocomplete="off">
    
        <br>  
        <h3>CALIBRAÇÃO:</h3>

        <p>
            <input type="submit" value="Iniciar">
        </p>   
        <p> <br></p>   

    </form>

  </div>
  </div>

  <iframe style="display:none" name="hidden-form">

  </iframe>
</body>

</html>



)rawliteral";



#endif


