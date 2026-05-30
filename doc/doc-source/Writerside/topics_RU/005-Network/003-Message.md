# 5.3. Сообщения

Сокеты в SimpleAPI в ответном пакете возвращают объект `PacketMessage`.

## Класс PacketMessage

Класс `PacketMessage` хранит поля:
- `m_ip_port` - адрес и порт отправителя
- `m_packet` - полученные данные

## Класс JsonMessage

  Класс `JsonMessage` аналогичен классу `PacketMessage`:
- `m_ip_port` - адрес и порт отправителя
- `m_json` - полученные данные в формате Json, если исходный PacketMessage можно было преобразовать в Json
