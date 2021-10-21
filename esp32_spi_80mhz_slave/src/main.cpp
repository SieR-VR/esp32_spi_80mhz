#include <Arduino.h>
#include <ESP32SPISlave.h>

ESP32SPISlave slave;

static constexpr uint32_t BUFFER_SIZE{32};
uint8_t spi_slave_tx_buf[BUFFER_SIZE];
uint8_t spi_slave_rx_buf[BUFFER_SIZE];

void set_buffer()
{
    for (uint32_t i = 0; i < BUFFER_SIZE; i++)
    {
        spi_slave_tx_buf[i] = (0xff - i);
    }

    memset(spi_slave_rx_buf, 0, BUFFER_SIZE);
}

constexpr uint8_t CORE_TASK_SPI_SLAVE {0};
constexpr uint8_t CORE_TASK_PROCESS_BUFFER {0};

static TaskHandle_t task_handle_wait_spi = 0;
static TaskHandle_t task_handle_process_buffer = 0;

void task_wait_spi(void *pvParameters)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        slave.wait(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);
        xTaskNotifyGive(task_handle_process_buffer);
    }
}

void task_process_buffer(void *pvParameters)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        for (uint32_t i = 0; i < BUFFER_SIZE; i++)
            printf("%d ", spi_slave_rx_buf[i]);
        printf("\n");

        slave.pop();
        xTaskNotifyGive(task_handle_wait_spi);
    }
}

void setup()
{
    Serial.begin(9600);
    delay(1000);

    slave.setDataMode(SPI_MODE0);
    slave.setQueueSize(1);
    slave.begin(VSPI);

    set_buffer();

    xTaskCreatePinnedToCore(
        task_wait_spi, 
        "task_wait_spi", 
        2048, 
        NULL, 
        2, 
        &task_handle_wait_spi, 
        CORE_TASK_SPI_SLAVE);
    
    xTaskNotifyGive(task_handle_wait_spi);

    xTaskCreatePinnedToCore(
        task_process_buffer, 
        "task_process_buffer", 
        2048, 
        NULL, 
        2, 
        &task_handle_process_buffer, 
        CORE_TASK_PROCESS_BUFFER);
}

void loop()
{
    // put your main code here, to run repeatedly:
}