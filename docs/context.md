# Контекст решения
<!-- Окружение системы (роли, участники, внешние системы) и связи системы с ним. Диаграмма контекста C4 и текстовое описание. 
-->
```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

Person(admin, "Администратор")
Person(user, "Пользователь")

System(conference_site, "Сервис поиска попутчиков", "Приложение для поиска попутчиков")



Rel(admin, conference_site, "Просмотр, добавление и редактирование информации о пользователях и поездках")
Rel(user, conference_site, "Регистрация, добавление, изменение, просмотр информации о поездках и маршрутах")



@enduml
```
## Назначение систем
|Система| Описание|
|-------|---------|
| Сервис поиска попутчиков|  Приложение, позволяющее найти попутчиков. Бэкенд сервиса реализован в виде микросервисной архитектуры |
