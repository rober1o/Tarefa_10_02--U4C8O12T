# Atividade - Conversores A/D

Este projeto foi desenvolvido para aprimorar o conhecimento sobre conversores A/D usando o **Raspberry Pi Pico W**. Foi realizado com o auxílio do simulador de eletrônica **Wokwi** e da placa **BitDogLab**.

## Componentes Utilizados

1. **LED RGB** GPIO 11, 12 e 13 (verde, azul e vermelho)
2. **3 RESISTORES** 330 ohms
3. **Microcontrolador Raspberry Pi Pico W**
4. **JOYSTICK** GPIO 22(botão), 26,27
5. **DISPLAY SSD1306** GPIO 14 e 15
6. **PUSHBUTTON** GPIO 5

## Funcionalidade

Ao iniciar o programa, o display apresentará uma borda e um quadrado no centro, que se moverá de acordo o Joystick, o led iniciará apagado.

### Deslocamento do Joystic

Caso o joystick sofra deslocamento no eixo y, o led azul do RGB aumentará sua luminoisidade de acordo for se afastando do centro, seja pra cima ou para baixo. O quadrado presente no display SSD1306 acompanhará o movimento de acordo com o joystick.

Caso o joystick sofra deslocamento no eixo x, o led vermelho do RGB aumentará sua luminoisidade de acordo for se afastando do centro, seja pra direita ou para esquerda.  O quadrado presente no display SSD1306 acompanhará o movimento de acordo com o joystick.

### Botão do Joystick

Caso pressionado o botão do Joystick alternará entre ligado e desligado o led Verde do RGB, além disso alternará entre três estilos diferentes de borda do display SSD1306.

### Botão A

Caso pressionado, o botão a desativará o PWM, impoossibilitando que os leds azul e vermelho brilhem com a movimentação do Joystick.

### Como Usar

#### Usando o Simulador Wokwi

- Clone este repositório: git clone https://github.com/rober1o/Tarefa_10_02--U4C8O12T.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Compilar o código e clicar no arquivo diagram.json.

#### Usando a BitDogLab

- Clone este repositório: git clone https://github.com/rober1o/Tarefa_10_02--U4C8O12T.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Compilar o projeto;
- Plugar a BitDogLab usando um cabo apropriado e gravar o código.

## Observação do teste na BitDogLab

Pode acontecer de ao testar o código na BitDogLab, o Joystick não esteja exatamente no centro, fazendo com que permaneça ligado mesmo estando em sua posiçaõ inicial. Para corrigir essa imperfeição, basta definir um valor na ZONA_MORTA para que o led fique desligado. No teste realizado no vídeo abaixo foi definido uma zona morta de 250, a fim de garantir estabilidade no joystick.

## Demonstração

<!-- TODO: adicionar link do vídeo -->
Vídeo demonstrando as funcionalidades da solução implementada: [Demonstração](https://youtu.be/0g13mYVv8dI)
