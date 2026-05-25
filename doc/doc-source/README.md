# Инструкция по компиляции документации:

Для создания документации в конечном виде следует использовать либо любую IDE от JetBrains с плагином WriterSide, либо их официальный docker-образ для автоматизации этого процесса

# Компиляция через графическую IDE
TODO:

## Компиляция документации
TODO:

# Компиляция через Docker-образ

## Установка образа

```bash
sudo apt install docker.io
docker create --name writerside_temp jetbrains/writerside-builder:latest
```

## Извлечение инструмента из docker-образа

```bash
sudo docker cp writerside_temp:/opt/builder ./writerside
```

## Компиляция документации
TODO:

