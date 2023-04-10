# Сервис поиска попутчиков

# Содержимое проекта
- ./docs - документация по проекту: uml диаграммы, OpenApi контракт - конфигурация endpoint.
- ./config - конфигурация БД
- ./database - менеджер подключений к БД, описание сущностей БД
- ./auth_service - реализация сервиса авторизации
- ./user_service - реализация сервиса пользователей
- ./trip_service - реализация сервиса поездок
- ./utils - набор вспомогательных функций
- .mariadb - контейнер БД со скриптом - main_script создания таблиц. Скрипт вызывается автоматически при первом запуске контейнера.
- .env - переменные окружения для запуска контейнеров
- .docker-compose.yaml - конфигурация запуска контейнеров
- main файлы контейнеров свервисов:
  - auth_main.cpp
  - user_main.cpp 
  - trip_main.cpp
- arch_lab.postman_collection.json - Postman коллекция с методами

# Запуск:
```
docker compose build
docker compose up
```

# Возможности
Для тестов можно использовать созданных в скрипте сущности:
- user: admin, password: 1234 - пользователь с правами админа
- user: test_user, password: 1234, id: 2 - пользователь с правами юзера
- для пользователя test_user есть один маршрут (route):
  - author_id: 2, name: "To home", points: ["Point 1", "Point 2"], id: 1
- у пользователя test_user есть одна поездка (trip):
  - author_id: 2, name: "From Point 1 to Point 2", description: "Hey there! I'm going from Point 1 to Point 2. Text me if you're too!", route_id: 1, date: now() при запуске скрипта
Также можно создавать свои сущности 