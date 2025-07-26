# Piscitec – Sistema Inteligente de Monitoreo y Alimentación para Peceras

**Universidad de Antioquia**  
Proyecto Final – Electrónica Digital III – 2025-1  
Autor: Duván Felipe Vélez Restrepo

---

## Descripción General

Piscitec es un sistema embebido desarrollado sobre la plataforma Raspberry Pi Pico W, orientado a la automatización del cuidado de peceras domésticas. Permite el monitoreo y control inteligente de variables como temperatura del agua, nivel, iluminación y disponibilidad de alimento, mediante sensores y actuadores integrados.

El sistema emplea una arquitectura modular y escalable, permite operación autónoma con bajo consumo energético, y está diseñado bajo un enfoque de eficiencia, mantenibilidad y robustez.

---

## Objetivo del Proyecto

Diseñar e implementar un sistema físico embebido que permita el monitoreo en tiempo real de parámetros críticos de una pecera, realizando acciones automáticas para mantener condiciones estables y saludables para los peces, sin intervención manual constante.

---

## Motivación del Proyecto

Elegimos desarrollar **Piscitec** porque representa una oportunidad ideal para integrar y aplicar los conocimientos adquiridos en este curso de *Electrónica Digital III*, así como en asignaturas complementarias como *Acondicionamiento de Señales*, *Electrónica Analógica I y II*, y *Circuitos I y II*. 

Este proyecto nos permitió trabajar sobre un problema real con impacto ambiental y doméstico, fortaleciendo habilidades prácticas en diseño modular de sistemas embebidos, programación en lenguaje C, uso de interrupciones, manejo de periféricos, control de actuadores y pruebas en tiempo real.

Además, la naturaleza interdisciplinaria del proyecto nos permitió abordar aspectos tanto de hardware como de software, reforzando la importancia de una documentación clara, una estructura de código mantenible y el uso eficiente de los recursos disponibles.

---

## Características Principales

- Medición en tiempo real:
  - Temperatura del agua (LM35)
  - Nivel de agua (HC-SR04)
  - Luz ambiental (LDR)
  - Verificación de alimento (sensor IR)
  - Detección de golpes o fallas (sensor de vibración NC)

- Control automático:
  - Calentador ON/OFF según histéresis
  - Iluminación por PWM (tira LED 12V)
  - Dispensador de alimento con servomotor SG80

- Interfaz:
  - Visualización en pantalla OLED 0.96'' (temperatura, alarmas, estado de alimentación)

- Alertas:
  - Buzzer y LED de advertencia ante eventos anómalos

---

## Requisitos Funcionales

- Lectura continua de sensores (temperatura, luz, nivel, comida, vibración)
- Control automático del calentador según histéresis de temperatura
- PWM en control de iluminación artificial
- Activación automática del dispensador de alimento
- Confirmación de disponibilidad de comida
- Alarmas por buzzer y LED
- Visualización local del estado del sistema en OLED
- Estructura de control basada en polling + interrupciones

---

## Requisitos No Funcionales

- Modularidad en software y hardware
- Código en lenguaje C estructurado por archivos
- Eficiencia energética (activación por condiciones)
- Escalabilidad (posibilidad de agregar Wi-Fi, app móvil o energía solar)
- Portabilidad (alimentación con batería LiPo)
- Alta disponibilidad y tolerancia a fallos

---

##  Flujo de Programa

El sistema implementa un ciclo de control basado en **polling** e **interrupciones**, estructurado de la siguiente forma:

### 1. Inicialización
- Configuración de periféricos (GPIO, PWM, I2C, ADC).
- Inicialización de la pantalla OLED y servomotor.
- Configuración de timers periódicos para tareas recurrentes.
- Activación de interrupciones para sensores (ultrasónico, IR, vibración).

### 2. Bucle Principal 
- Lectura periódica de sensores:
  - Temperatura (LM35)
  - Luz ambiental (LDR)
  - Nivel de agua (HC-SR04 con media móvil)
  - Sensor infrarrojo (comida presente/ausente)
  - Sensor de vibración (eventos físicos)

- Evaluación de condiciones ambientales y actualización de flags.
- Activación de actuadores:
  - Calentador ON/OFF por histéresis
  - Iluminación PWM según nivel de luz
  - Alimentación de peces con servomotor
  - Activación de buzzer ante vibración
- Visualización de estado en pantalla OLED:
  - Temperatura, nivel de agua, luz, IR, vibración

### 3. Interrupciones y Alarmas
- **Sensor IR (LOW_FOOD_PIN):** detección de alimento.
- **Sensor ultrasónico (ECHO_PIN):** medición de distancia por flancos.
- **Sensor de vibración:** genera alerta inmediata.
- **Timers y alarmas:**
  - `add_repeating_timer_ms`: tareas periódicas (sensado, trigger)
  - `add_alarm_in_ms`: apagado de buzzer, ciclos de alimentación

### 4. Modularidad
- Cada funcionalidad está contenida en módulos independientes:
  - `temperature.c`, `lights.c`, `food.c`, `oled_display.c`, etc.
- Organización clara del flujo de eventos y separación por hardware.

## Presupuesto del Sistema

### Prototipo individual (estimado)

| Componente                              | Cantidad | Precio unitario (USD) | Subtotal (USD) |
|-----------------------------------------|----------|------------------------|----------------|
| Sensor de temperatura LM35              | 1        | 2.47                   | 2.47           |
| Sensor ultrasónico HC-SR04             | 1        | 1.23                   | 1.23           |
| Sensor de luz (LDR + resistencia)       | 1        | 0.63                   | 0.63           |
| Sensor IR (emisor + receptor)           | 1        | 0.80                   | 0.80           |
| Sensor de vibración NC-ECO              | 1        | 0.90                   | 0.90           |
| Servomotor SG80                         | 1        | 2.20                   | 2.20           |
| Pantalla OLED 0.96” I2C                 | 1        | 3.55                   | 3.55           |
| Transistor 2N2222                       | 1        | 0.24                   | 0.24           |
| LED de estado + Buzzer                  | 1        | 0.35                   | 0.35           |
| Raspberry Pi Pico W                     | 1        | 11.00                  | 11.00          |
| Resistencia de potencia 480W (SB-608)   | 1        | 3.75                   | 3.75           |
| Tira LED 12V (30 cm aprox)              | 1        | 3.25                   | 3.25           |
| Batería LiPo 4.1 V                      | 4        | 3.25                   | 13.00          |
| Caja plástica + cables + perfoboard     | 1        | 20.00                  | 20.00          |
| **Total estimado (prototipo)**          | —        | —                      | **63.37 USD**  |

---

### Producción en masa (100 unidades)

| Componente                              | Precio mayorista (USD) | Subtotal x100 (USD) |
|-----------------------------------------|-------------------------|---------------------|
| Sensor de temperatura LM35              | 0.10                    | 10.00               |
| Sensor ultrasónico HC-SR04             | 0.99                    | 99.00               |
| Sensor de luz (LDR + resistencia)       | 0.12                    | 12.00               |
| Sensor IR (emisor + receptor)           | 0.40                    | 40.00               |
| Sensor de vibración NC-ECO              | 0.90                    | 90.00               |
| Servomotor SG80                         | 1.45                    | 145.00              |
| Pantalla OLED 0.96” I2C                 | 2.84                    | 284.00              |
| Transistor 2N2222                       | 0.099                   | 9.90                |
| LED de estado + Buzzer                  | 0.20                    | 20.00               |
| Raspberry Pi Pico W                     | 2.00                    | 200.00              |
| Resistencia de potencia 480W (SB-608)   | 2.50                    | 250.00              |
| Tira LED 12V (30 cm aprox)              | 0.99                    | 99.00               |
| Batería LiPo 4.1 V                      | 2.22                    | 888.00              |
| Caja plástica + cables + perfoboard     | 12.00                   | 1200.00             |
| **Total estimado (100 unidades)**       | —                       | **3346.90 USD**     |


Costo por unidad: **~USD $63.37 USD**
Costo promedio por unidad fabricando 100: **33.47 USD**

> Fuente: archivo `Presupuesto_Piscitec_V3.xlsx`

---

## 📁 Estructura del Proyecto

El código fuente del sistema se encuentra organizado en una única carpeta llamada `source/`, que contiene todos los archivos `.c` y `.h` correspondientes a los distintos módulos funcionales del sistema.
