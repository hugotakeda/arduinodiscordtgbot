# Monitor de Geladeira com ESP32-CAM  

## Visão Geral  

O **Monitor de Geladeira com ESP32-CAM** é um projeto que utiliza um módulo ESP32-CAM equipado com uma câmera OV2640 e um sensor PIR (Infravermelho Passivo) para detectar movimento próximo a uma geladeira. Quando o movimento é detectado, o sistema acorda, exibe um aviso em um display OLED SSD1306, captura uma foto e a envia para um servidor via HTTP POST. Após concluir as tarefas, o ESP32 entra em modo *deep sleep* para economizar energia.  

Este projeto foi desenvolvido para monitorar acessos não autorizados à geladeira, com aviso visual e funcionalidade de captura de fotos.  

## Funcionalidades  

- **Detecção de Movimento**: Utiliza um sensor PIR para detectar movimento próximo à geladeira.  
- **Aviso Visual**: Exibe uma contagem regressiva e mensagens de alerta em um display OLED SSD1306 (128x64).  
- **Captura de Foto**: Tira fotos usando o módulo de câmera OV2640 do ESP32-CAM.  
- **Envio via HTTP POST**: Envia as imagens capturadas para uma URL de servidor especificada.  
- **Modo Deep Sleep**: Entra em modo de baixo consumo após a execução das tarefas para economizar energia.  
- **Conexão Wi-Fi**: Conecta-se a uma rede Wi-Fi para enviar as imagens.  

## Requisitos de Hardware  

- **Módulo ESP32-CAM**: Placa de desenvolvimento com módulo ESP32-S e câmera OV2640.  
- **Sensor PIR**: Conectado ao GPIO 33 para detecção de movimento.  
- **Display OLED SSD1306**: Resolução 128x64, conectado via I2C (SCL: GPIO 22, SDA: GPIO 21).  
- **Fonte de Alimentação**: USB ou bateria (garanta um fornecimento estável de 3.3V ou 5V).  
- **Rede Wi-Fi**: Uma rede Wi-Fi local para conectividade com a internet.  

## Requisitos de Software  

- **Arduino IDE**: Para programar o ESP32-CAM.  
- **Suporte à Placa ESP32**: Adicione o pacote da placa ESP32 à Arduino IDE.  
- **Bibliotecas Necessárias**:  
  - `WiFi.h`: Para conexão Wi-Fi (incluída no pacote da placa ESP32).  
  - `esp_http_client.h`: Para requisições HTTP POST (incluída no pacote da placa ESP32).  
  - `U8x8lib.h`: Para controle do display OLED SSD1306 (instale via Arduino Library Manager).  
  - `OV2640.h`: Biblioteca personalizada para a câmera OV2640 (certifique-se de que está na pasta `src`).  
- **Servidor**: Um servidor web rodando PHP (ou outra linguagem) para receber as imagens (veja a seção **Configuração do Servidor**).  

## Instalação  

### Configurando a Arduino IDE  

1. **Instale a Arduino IDE** a partir do site [arduino.cc](https://www.arduino.cc/).  
2. **Adicione o suporte à placa ESP32**:  
   - Vá em *Arquivo > Preferências* e adicione a seguinte URL em *"Additional Boards Manager URLs"*:  
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/master/package_esp32_index.json  
     ```  
   - Em *Ferramentas > Placa > Gerenciador de Placas*, procure por "ESP32" e instale o pacote.  
   - Selecione a placa: *Ferramentas > Placa > ESP32 Arduino > ESP32-CAM*.  

### Instalando Bibliotecas  

1. **Instale a biblioteca U8x8lib**:  
   - Vá em *Sketch > Incluir Biblioteca > Gerenciar Bibliotecas*, procure por "U8g2" e instale.  
2. **Verifique se a biblioteca `OV2640.h` está na pasta `src` do seu projeto**.  

### Configurando o Código  

1. Abra o sketch Arduino fornecido (`fridge_monitor.ino`) na Arduino IDE.  
2. Atualize as seguintes variáveis no código:  
   ```cpp
   const char *ssid = "VIVOFIBRA-1FE1"; // Substitua pelo seu SSID Wi-Fi  
   const char *password = "2NBJwUFuUX"; // Substitua pela sua senha Wi-Fi  
   const char *post_url = "http://192.168.15.10:8888/esp32cam/posted.php"; // Substitua pela URL do seu servidor  
   ```  
3. Certifique-se de que `post_url` aponte para o seu servidor (veja **Configuração do Servidor**).  

### Upload do Código  

1. Conecte o ESP32-CAM ao computador via adaptador USB-serial (ex: módulo FTDI).  
2. Selecione a porta correta em *Ferramentas > Porta*.  
3. Faça o upload do sketch para o ESP32-CAM.  

## Configuração do Servidor  

O ESP32-CAM envia imagens via HTTP POST para um servidor. Abaixo está um exemplo de configuração usando PHP em uma máquina local com MAMP (macOS).  

### Pré-requisitos  

- **MAMP**: Baixe e instale a partir de [mamp.info](https://www.mamp.info/).  
- **PHP**: Incluído no MAMP.  
- **Diretório**: `/Applications/MAMP/htdocs`.  

### Passos  

1. **Inicie o MAMP**:  
   - Abra o MAMP e inicie o servidor Apache (porta padrão: 8888).  
   - Confirme que o servidor está rodando (luz verde no MAMP).  

2. **Crie o Script PHP**:  
   - Em `/Applications/MAMP/htdocs`, crie uma pasta chamada `esp32cam`.  
   - Dentro dela, crie um arquivo chamado `posted.php` com o seguinte conteúdo:  
     ```php
     <?php
     header('Content-Type: text/plain');

     $upload_dir = 'uploads/';
     if (!is_dir($upload_dir)) {
         mkdir($upload_dir, 0777, true);
     }

     $filename = $upload_dir . 'image_' . time() . '.jpg';
     $image_data = file_get_contents('php://input');

     if ($image_data !== false && !empty($image_data)) {
         if (file_put_contents($filename, $image_data)) {
             echo "Imagem enviada com sucesso: $filename";
         } else {
             echo "Falha ao salvar a imagem.";
         }
     } else {
         echo "Nenhum dado de imagem recebido.";
     }
     ?>
     ```  
   - Crie uma pasta chamada `uploads` em `/Applications/MAMP/htdocs/esp32cam`.  
   - Defina as permissões:  
     ```sh
     chmod -R 777 /Applications/MAMP/htdocs/esp32cam/uploads
     ```  

3. **Teste o Servidor**:  
   - Acesse `http://localhost:8888/esp32cam/posted.php` em um navegador. Você deve ver a mensagem *"Nenhum dado de imagem recebido."*.  
   - Atualize a variável `post_url` no código Arduino para corresponder ao IP do seu servidor (ex: `http://192.168.15.10:8888/esp32cam/posted.php`).  

### Implantação Online (Opcional)  

- Para acesso remoto, use um serviço de hospedagem ou configure *port forwarding* no seu roteador.  
- Considere adicionar segurança (ex: HTTPS, chave de autenticação).  

## Como Usar  

1. **Alimente o ESP32-CAM**:  
   - Conecte o ESP32-CAM a uma fonte de energia (USB ou bateria).  
   - O dispositivo entrará em *deep sleep* e aguardará a detecção de movimento pelo sensor PIR.  

2. **Detecção de Movimento**:  
   - Quando o sensor PIR (GPIO 33) detectar movimento, o ESP32 acordará.  
   - O display OLED exibirá um aviso (*"AFASTE-SE DA GELADEIRA"*) seguido de uma contagem regressiva de 5 segundos.  

3. **Captura e Envio da Foto**:  
   - Se o movimento persistir por 3 segundos, a câmera capturará uma foto.  
   - A foto será enviada ao servidor via HTTP POST.  
   - O dispositivo retornará ao modo *deep sleep*.  

4. **Visualização das Imagens**:  
   - Verifique a pasta `/Applications/MAMP/htdocs/esp32cam/uploads` para ver as imagens salvas.  
   - Alternativamente, configure uma interface web para visualizar as imagens remotamente.  

## Solução de Problemas  

### Falha na Conexão Wi-Fi  

- Verifique o SSID e senha no código.  
- Verifique o monitor serial (`Serial.begin(115200)`) para ver o status da conexão.  
- Certifique-se de que o ESP32-CAM está dentro do alcance do Wi-Fi.  

### Imagens Não Estão Sendo Salvas  

- Confirme se o servidor está rodando e se a `post_url` está correta.  
- Verifique os logs do MAMP:  
  - `/Applications/MAMP/logs/php_error.log`  
  - `/Applications/MAMP/logs/apache_error.log`  
- Verifique as permissões da pasta `uploads` (`chmod -R 777`).  

### Problemas com o Sensor PIR  

- Certifique-se de que o sensor PIR está conectado ao GPIO 33 e configurado como entrada.  
- Teste o sensor monitorando `digitalRead(PIR_PIN)` no monitor serial.  

### Display OLED Não Funciona  

- Verifique as conexões I2C (SCL: GPIO 22, SDA: GPIO 21).  
- Confirme se a biblioteca `U8x8lib` foi instalada corretamente.  

## Considerações de Segurança  

- **Autenticação**: Adicione uma chave secreta ao script PHP e ao código do ESP32 para evitar acesso não autorizado:  
  - **PHP**:  
    ```php
    $secret_key = "sua_chave_secreta";
    if ($_SERVER['HTTP_X_SECRET_KEY'] !== $secret_key) {
        echo "Chave inválida.";
        exit;
    }
    ```  
  - **ESP32**:  
    ```cpp
    esp_http_client_set_header(http_client, "X-Secret-Key", "sua_chave_secreta");
    ```  
- **HTTPS**: Use um servidor seguro com SSL para implantações online.  
- **Firewall**: Certifique-se de que a porta do servidor (ex: 8888) esteja aberta, mas protegida.  

## Melhorias Futuras  

- Adicionar uma interface web para visualizar as imagens capturadas.  
- Integrar notificações (ex: e-mail ou Telegram) quando movimento for detectado.  
- Armazenar imagens em nuvem (ex: AWS S3, Google Cloud Storage).  
- Implementar transmissão em tempo real em vez de fotos únicas.  

## Licença  

Este projeto está licenciado sob a **MIT License**. Consulte o arquivo `LICENSE` para mais detalhes.  

## Agradecimentos  

- **ESP32 Arduino Core** pelo suporte ao ESP32.  
- **Biblioteca U8g2** pelo controle do display OLED.  
- **MAMP** pela configuração local do servidor em macOS.
