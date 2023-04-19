create table user (
    id int not null,
    login varchar(128) not null unique,
    password varchar(256) not null,
    role varchar(64) not null,
    first_name varchar(256) not null,
    last_name varchar(256) not null,
    email varchar(128) not null unique,
    tel varchar(20) not null,
    deleted boolean not null default false,
    primary key(id)
);

-- insert into user (login, password, role, first_name, last_name, email, tel) values
-- ('admin', '1234', 'admin', 'admin', 'admin', 'admin@admin.com', '+71234567890'),
-- ('test_user', '1234', 'user', 'Name1', 'Surname1', 'user@user.com', '+72345678901'),
-- ('test_user2', '1234', 'user2', 'Name2', 'Surname2', 'user2@user2.com', '+73456789012');

-- create table route (
--     id int not null AUTO_INCREMENT,
--     author_id int not null references user(id),
--     name varchar(128),
--     points JSON not null,
--     deleted boolean not null default false,
--     primary key(id)
-- );

-- insert into route (name, points, author_id)
--     select
--         'To home',
--         '["Point 1", "Point 2"]',
--         id
--     from user where login = 'test_user';

-- create table trip (
--     id int not null AUTO_INCREMENT,
--     author_id int not null references user(id),
--     name varchar(256) not null,
--     description varchar(512) not null,
--     route_id int not null references route(id),
--     date datetime not null,
--     creation_date datetime not null default now(),
--     deleted boolean not null default false,
--     primary key (id)
-- );

-- insert into trip (author_id, name, description, route_id, date)
--     select
--         author_id,
--         'From Point 1 to Point 2',
--         "Hey there! I'm going from Point 1 to Point 2. Text me if you're too!",
--         id,
--         now()
--     from route where name = 'To home';

-- create table passengers (
--     id int not null AUTO_INCREMENT,
--     passenger_id int not null references user(id),
--     trip_id int not null references trip(id),
--     primary key (id)
-- );

-- insert into passengers (passenger_id, trip_id) values
--     ((select id from user where login = 'test_user2'),
--     (select t.id from trip t join user u on u.id = t.author_id where u.login = 'test_user'));