#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"

#define BOTAO_A 5
#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13
#define EIXO_Y 26
#define EIXO_X 27
#define BOTAO_JOYSTICK 22
#define PWM_LIMITE 4095
#define PWM_DIVISOR 30.52f
#define PORTA_I2C i2c1
#define PINO_I2C_SDA 14
#define PINO_I2C_SCL 15
#define ENDERECO_I2C 0x3C
#define ZONA_MORTA 0  // Define a faixa onde os LEDs permanecerão apagados
#define DEBOUNCE_INTERVALO 200000  // 200 milissegundos (200000 microssegundos)

bool state_led_verde = false;
bool led_joystick = true;
absolute_time_t ultima_interrupcao = 0;
ssd1306_t display_oled;
int borda = 0;
const int quadrado = 8;
int centro_x;
int centro_y;

// Função para verificar o debounce
bool verificar_debounce() {
    absolute_time_t agora = get_absolute_time();
    if (absolute_time_diff_us(ultima_interrupcao, agora) < DEBOUNCE_INTERVALO) {
        return false;
    }
    ultima_interrupcao = agora;
    return true;
}

// Função para inicializar o PWM
void inicializar_pwm(uint pino, uint limite, float divisor) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, limite);
    pwm_config_set_clkdiv(&config, divisor);
    pwm_init(slice, &config, true);
}

// Mapeamento do eixo Y com zona morta
int16_t delocamento_y(uint16_t valor) {
    int16_t deslocamento = (valor < 2047) ? (2047 - valor) : (valor - 2047);
    return (deslocamento < ZONA_MORTA) ? 0 : deslocamento; 
}

// Mapeamento do eixo X com zona morta
int16_t delocamento_x(uint16_t valor) {
    int16_t deslocamento = (valor < 2047) ? (2047 - valor) : (valor - 2047);
    return (deslocamento < ZONA_MORTA) ? 0 : deslocamento; 
}

// Desenha bordas no display
void atualizar_display() {
    ssd1306_config(&display_oled);
    ssd1306_fill(&display_oled, false);

    switch (borda) {
        case 0:
            ssd1306_rect(&display_oled, 3, 3, 122, 60, true, false);
            break;
        case 1:
            ssd1306_rect(&display_oled, 3, 3, 122, 58, true, false);
            ssd1306_rect(&display_oled, 5, 5, 118, 54, true, false);
            break;
        case 2:
            ssd1306_rect(&display_oled, 3, 3, 122, 56, true, false);
            ssd1306_rect(&display_oled, 5, 5, 118, 52, true, false);
            ssd1306_rect(&display_oled, 7, 7, 114, 48, true, false);
            break;
    }
}

// Callback dos botões
void callback_botoes(uint pino, uint32_t eventos) {
    if (!verificar_debounce()) return;  // 200ms de debounce

    if (pino == BOTAO_JOYSTICK) {
        state_led_verde = !state_led_verde;
        gpio_put(LED_VERDE, state_led_verde);
        borda = (borda + 1) % 3;
        atualizar_display();
    } else if (pino == BOTAO_A) {
        led_joystick = !led_joystick;
        pwm_set_gpio_level(LED_AZUL, led_joystick ? delocamento_y(adc_read()) : 0);
        pwm_set_gpio_level(LED_VERMELHO, led_joystick ? delocamento_x(adc_read()) : 0);
    }
}

// Inicializa hardware e periféricos
void inicializar_hardware(){
    inicializar_pwm(LED_AZUL, PWM_LIMITE, PWM_DIVISOR);
    inicializar_pwm(LED_VERMELHO, PWM_LIMITE, PWM_DIVISOR);

    adc_init();
    adc_gpio_init(EIXO_Y);
    adc_gpio_init(EIXO_X);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(BOTAO_JOYSTICK);
    gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(BOTAO_JOYSTICK);

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    gpio_set_irq_enabled_with_callback(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &callback_botoes);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &callback_botoes);
    
    i2c_init(PORTA_I2C, 400 * 1000);
    gpio_set_function(PINO_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_I2C_SDA);
    gpio_pull_up(PINO_I2C_SCL);

    // Inicializa o display OLED
    ssd1306_init(&display_oled, 128, 64, false, ENDERECO_I2C, PORTA_I2C);
    centro_x = (WIDTH - quadrado) / 2;
    centro_y = (HEIGHT - quadrado) / 2;
}

int main() {
    inicializar_hardware();

    while (true) {
        adc_select_input(0);
        uint16_t valor_y = adc_read();
        adc_select_input(1);
        uint16_t valor_x = adc_read();

        if (led_joystick) {
            pwm_set_gpio_level(LED_AZUL, delocamento_y(valor_y));
            pwm_set_gpio_level(LED_VERMELHO, delocamento_x(valor_x));
        } else {
            pwm_set_gpio_level(LED_AZUL, 0);
            pwm_set_gpio_level(LED_VERMELHO, 0);
        }

        int pos_x = centro_x + ((2048 - (int)valor_x) * centro_x) / 2048;
        int pos_y = centro_y + ((2048 - (int)valor_y) * centro_y) / 2048;

        atualizar_display();
        ssd1306_rect(&display_oled, pos_y, pos_x, quadrado, quadrado, true, true);
        ssd1306_send_data(&display_oled);
    }
}
