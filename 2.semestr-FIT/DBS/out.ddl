-- Generated by Oracle SQL Developer Data Modeler 19.2.0.182.1216
--   at:        2020-04-09 13:28:08 CEST
--   site:      Oracle Database 11g
--   type:      Oracle Database 11g

prompt #---------------------#
prompt #- Pomocne procedury -#
prompt #---------------------#

create or replace procedure SMAZ_VSECHNY_TABULKY AS
-- pokud v logu bude uvedeno, ze nektery objekt nebyl zrusen, protoze na nej jiny jeste existujici objekt stavi, spust proceduru opakovane, dokud se nezrusi vse
begin
  for iRec in 
    (select distinct OBJECT_TYPE, OBJECT_NAME,
      'drop '||OBJECT_TYPE||' "'||OBJECT_NAME||'"'||
      case OBJECT_TYPE when 'TABLE' then ' cascade constraints purge' else ' ' end as PRIKAZ
    from USER_OBJECTS where OBJECT_NAME not in ('SMAZ_VSECHNY_TABULKY', 'VYPNI_CIZI_KLICE', 'ZAPNI_CIZI_KLICE', 'VYMAZ_DATA_VSECH_TABULEK')
    ) loop
        begin
          dbms_output.put_line('Prikaz: '||irec.prikaz);
        execute immediate iRec.prikaz;
        exception
          when others then dbms_output.put_line('NEPOVEDLO SE!');
        end;
      end loop;
end;
/

create or replace procedure VYPNI_CIZI_KLICE as 
begin
  for cur in (select CONSTRAINT_NAME, TABLE_NAME from USER_CONSTRAINTS where CONSTRAINT_TYPE = 'R' ) 
  loop
    execute immediate 'alter table '||cur.TABLE_NAME||' modify constraint "'||cur.CONSTRAINT_NAME||'" DISABLE';
  end loop;
end VYPNI_CIZI_KLICE;
/


create or replace procedure ZAPNI_CIZI_KLICE as 
begin
  for cur in (select CONSTRAINT_NAME, TABLE_NAME from USER_CONSTRAINTS where CONSTRAINT_TYPE = 'R' ) 
  loop
    execute immediate 'alter table '||cur.TABLE_NAME||' modify constraint "'||cur.CONSTRAINT_NAME||'" enable validate';
  end loop;
end ZAPNI_CIZI_KLICE;
/

create or replace procedure VYMAZ_DATA_VSECH_TABULEK is
begin
  -- Vymazat data vsech tabulek
  VYPNI_CIZI_KLICE;
  for v_rec in (select distinct TABLE_NAME from USER_TABLES)
  loop
    execute immediate 'truncate table '||v_rec.TABLE_NAME||' drop storage';
  end loop;
  ZAPNI_CIZI_KLICE;
  
  -- Nastavit vsechny sekvence od 1
  for v_rec in (select distinct SEQUENCE_NAME  from USER_SEQUENCES)
  loop
    execute immediate 'alter sequence '||v_rec.SEQUENCE_NAME||' restart start with 1';
  end loop;
end VYMAZ_DATA_VSECH_TABULEK;
/

prompt #------------------------#
prompt #- Zrusit stare tabulky -#
prompt #------------------------#

exec SMAZ_VSECHNY_TABULKY;

CREATE TABLE bedding (
    bedding_ean        NUMBER NOT NULL,
    pattern            VARCHAR2(20 CHAR),
    washed             CHAR(1) NOT NULL,
    person_person_id   NUMBER
);

CREATE UNIQUE INDEX bedding__idx ON
    bedding (
        person_person_id
    ASC );

ALTER TABLE bedding ADD CONSTRAINT bedding_pk PRIMARY KEY ( bedding_ean );

CREATE TABLE bike (
    manufacturer                VARCHAR2(20 BYTE),
    manufacture_date            DATE,
    bike_size                   VARCHAR2(5 CHAR),
    bike_id                     NUMBER NOT NULL,
    bike_storage_room_id        NUMBER,
    bike_storage_building_id    NUMBER,
    bike_storage_floor_number   NUMBER,
    bike_type                   VARCHAR2(20 BYTE)
);

ALTER TABLE bike ADD CONSTRAINT bike_pk PRIMARY KEY ( bike_id );

CREATE TABLE bike_storage (
    building_id         NUMBER NOT NULL,
    floor_number        NUMBER NOT NULL,
    room_id             NUMBER NOT NULL,
    person_person_id    NUMBER NOT NULL,
    bike_storage_type   VARCHAR2(12) NOT NULL
);

ALTER TABLE bike_storage
    ADD CONSTRAINT ch_inh_bike_storage CHECK ( bike_storage_type IN (
        'Bike_storage'
    ) );

ALTER TABLE bike_storage
    ADD CONSTRAINT bike_storage_pk PRIMARY KEY ( room_id,
                                                 building_id,
                                                 floor_number );

CREATE TABLE buidling (
    mark          CHAR(5 CHAR) NOT NULL,
    height        NUMBER,
    building_id   NUMBER NOT NULL
);

ALTER TABLE buidling ADD CONSTRAINT buidling_pk PRIMARY KEY ( building_id );

CREATE TABLE floor (
    floor_number           NUMBER NOT NULL,
    buidling_building_id   NUMBER NOT NULL
);

ALTER TABLE floor ADD CONSTRAINT floor_pk PRIMARY KEY ( buidling_building_id,
                                                        floor_number );

CREATE TABLE furniture (
    material            VARCHAR2(20 CHAR),
    condition           VARCHAR2(15 CHAR),
    furniture_ean       NUMBER NOT NULL,
    furniture_type      VARCHAR2(25 CHAR) NOT NULL,
    room_room_id        NUMBER,
    room_building_id    NUMBER,
    room_floor_number   NUMBER
);

ALTER TABLE furniture
    ADD CHECK ( condition IN (
        'Factory new',
        'Minimal wear',
        'Well-worn'
    ) );

ALTER TABLE furniture ADD CONSTRAINT furniture_pk PRIMARY KEY ( furniture_ean );

CREATE TABLE person (
    surname               VARCHAR2(20 CHAR) NOT NULL,
    age                   NUMBER,
    firstname             VARCHAR2(20 CHAR) NOT NULL,
    gender                VARCHAR2(15 CHAR),
    person_id             NUMBER NOT NULL,
    bike_bike_id          NUMBER,
    bedding_bedding_ean   NUMBER
);

ALTER TABLE person ADD CHECK ( age BETWEEN 18 AND 150 );

ALTER TABLE person
    ADD CHECK ( gender IN (
        'Female',
        'Male',
        'Non-defined'
    ) );

CREATE UNIQUE INDEX person__idx ON
    person (
        bedding_bedding_ean
    ASC );

ALTER TABLE person ADD CONSTRAINT person_pk PRIMARY KEY ( person_id );

CREATE TABLE residence (
    residance_start_date          DATE NOT NULL,
    price_month                   NUMBER NOT NULL,
    person_person_id              NUMBER NOT NULL,
    residence_id                  NUMBER NOT NULL,
    residence_end_date            DATE,
    residence_room_room_id        NUMBER NOT NULL,
    residence_room_building_id    NUMBER NOT NULL,
    residence_room_floor_number   NUMBER NOT NULL
);

ALTER TABLE residence ADD CONSTRAINT residence_pk PRIMARY KEY ( residence_id );

CREATE TABLE residence_room (
    building_id    NUMBER NOT NULL,
    floor_number   NUMBER NOT NULL,
    room_id        NUMBER NOT NULL,
    area           NUMBER NOT NULL,
    condition      VARCHAR2(15 BYTE) NOT NULL
);

ALTER TABLE residence_room
    ADD CHECK ( condition IN (
        'Factory new',
        'Minimal wear',
        'Well-worn'
    ) );

ALTER TABLE residence_room
    ADD CONSTRAINT residence_room_pk PRIMARY KEY ( room_id,
                                                   building_id,
                                                   floor_number );

CREATE TABLE room (
    floor_buidling_building_id   NUMBER NOT NULL,
    floor_floor_number           NUMBER NOT NULL,
    room_id                      NUMBER NOT NULL,
    capacity                     NUMBER NOT NULL,
    room_type                    VARCHAR2(14) NOT NULL
);

ALTER TABLE room
    ADD CONSTRAINT ch_inh_room CHECK ( room_type IN (
        'Bike_storage',
        'Residence_Room'
    ) );

ALTER TABLE room
    ADD CONSTRAINT room_pk PRIMARY KEY ( room_id,
                                         floor_buidling_building_id,
                                         floor_floor_number );

CREATE TABLE school (
    city        VARCHAR2(50 CHAR) NOT NULL,
    street      VARCHAR2(50 CHAR),
    postcode    VARCHAR2(30 CHAR),
    name        VARCHAR2(50 CHAR) NOT NULL,
    major       VARCHAR2(20 CHAR) NOT NULL,
    school_id   NUMBER NOT NULL
);

ALTER TABLE school ADD CONSTRAINT school_pk PRIMARY KEY ( school_id );

CREATE TABLE school_attendance (
    degree                NUMBER,
    grade_point_avergae   NUMBER,
    attendance_id         NUMBER NOT NULL,
    person_person_id      NUMBER NOT NULL,
    school_school_id      NUMBER NOT NULL,
    attendance_start      DATE NOT NULL,
    attendance_end        DATE
);

ALTER TABLE school_attendance ADD CONSTRAINT school_attendance_pk PRIMARY KEY ( attendance_id );

ALTER TABLE bedding
    ADD CONSTRAINT bedding_person_fk FOREIGN KEY ( person_person_id )
        REFERENCES person ( person_id );

ALTER TABLE bike
    ADD CONSTRAINT bike_bike_storage_fk FOREIGN KEY ( bike_storage_room_id,
                                                      bike_storage_building_id,
                                                      bike_storage_floor_number )
        REFERENCES bike_storage ( room_id,
                                  building_id,
                                  floor_number );

ALTER TABLE bike_storage
    ADD CONSTRAINT bike_storage_person_fk FOREIGN KEY ( person_person_id )
        REFERENCES person ( person_id );

ALTER TABLE bike_storage
    ADD CONSTRAINT bike_storage_room_fk FOREIGN KEY ( room_id,
                                                      building_id,
                                                      floor_number )
        REFERENCES room ( room_id,
                          floor_buidling_building_id,
                          floor_floor_number );

ALTER TABLE floor
    ADD CONSTRAINT floor_buidling_fk FOREIGN KEY ( buidling_building_id )
        REFERENCES buidling ( building_id );

ALTER TABLE furniture
    ADD CONSTRAINT furniture_room_fk FOREIGN KEY ( room_room_id,
                                                   room_building_id,
                                                   room_floor_number )
        REFERENCES room ( room_id,
                          floor_buidling_building_id,
                          floor_floor_number );

ALTER TABLE person
    ADD CONSTRAINT person_bedding_fk FOREIGN KEY ( bedding_bedding_ean )
        REFERENCES bedding ( bedding_ean );

ALTER TABLE person
    ADD CONSTRAINT person_bike_fk FOREIGN KEY ( bike_bike_id )
        REFERENCES bike ( bike_id );

ALTER TABLE residence
    ADD CONSTRAINT residence_person_fk FOREIGN KEY ( person_person_id )
        REFERENCES person ( person_id );

ALTER TABLE residence
    ADD CONSTRAINT residence_residence_room_fk FOREIGN KEY ( residence_room_room_id,
                                                             residence_room_building_id,
                                                             residence_room_floor_number )
        REFERENCES residence_room ( room_id,
                                    building_id,
                                    floor_number );

ALTER TABLE residence_room
    ADD CONSTRAINT residence_room_room_fk FOREIGN KEY ( room_id,
                                                        building_id,
                                                        floor_number )
        REFERENCES room ( room_id,
                          floor_buidling_building_id,
                          floor_floor_number );

ALTER TABLE room
    ADD CONSTRAINT room_floor_fk FOREIGN KEY ( floor_buidling_building_id,
                                               floor_floor_number )
        REFERENCES floor ( buidling_building_id,
                           floor_number );

ALTER TABLE school_attendance
    ADD CONSTRAINT school_attendance_person_fk FOREIGN KEY ( person_person_id )
        REFERENCES person ( person_id );

ALTER TABLE school_attendance
    ADD CONSTRAINT school_attendance_school_fk FOREIGN KEY ( school_school_id )
        REFERENCES school ( school_id );

CREATE OR REPLACE TRIGGER arc_fkarc_4_bike_storage BEFORE
    INSERT OR UPDATE OF room_id, building_id, floor_number ON bike_storage
    FOR EACH ROW
DECLARE
    d VARCHAR2(14);
BEGIN
    SELECT
        a.room_type
    INTO d
    FROM
        room a
    WHERE
        a.room_id = :new.room_id
        AND a.floor_buidling_building_id = :new.building_id
        AND a.floor_floor_number = :new.floor_number;

    IF ( d IS NULL OR d <> 'Bike_storage' ) THEN
        raise_application_error(-20223, 'FK Bike_storage_Room_FK in Table Bike_storage violates Arc constraint on Table Room - discriminator column Room_TYPE doesn''t have value ''Bike_storage'''
        );
    END IF;

EXCEPTION
    WHEN no_data_found THEN
        NULL;
    WHEN OTHERS THEN
        RAISE;
END;
/

CREATE OR REPLACE TRIGGER arc_fkarc_4_residence_room BEFORE
    INSERT OR UPDATE OF room_id, building_id, floor_number ON residence_room
    FOR EACH ROW
DECLARE
    d VARCHAR2(14);
BEGIN
    SELECT
        a.room_type
    INTO d
    FROM
        room a
    WHERE
        a.room_id = :new.room_id
        AND a.floor_buidling_building_id = :new.building_id
        AND a.floor_floor_number = :new.floor_number;

    IF ( d IS NULL OR d <> 'Residence_Room' ) THEN
        raise_application_error(-20223, 'FK Residence_Room_Room_FK in Table Residence_Room violates Arc constraint on Table Room - discriminator column Room_TYPE doesn''t have value ''Residence_Room'''
        );
    END IF;

EXCEPTION
    WHEN no_data_found THEN
        NULL;
    WHEN OTHERS THEN
        RAISE;
END;
/

CREATE SEQUENCE bike_bike_id_seq START WITH 1 NOCACHE ORDER;

CREATE OR REPLACE TRIGGER bike_bike_id_trg BEFORE
    INSERT ON bike
    FOR EACH ROW
    WHEN ( new.bike_id IS NULL )
BEGIN
    :new.bike_id := bike_bike_id_seq.nextval;
END;
/

CREATE SEQUENCE person_person_id_seq START WITH 1 NOCACHE ORDER;

CREATE OR REPLACE TRIGGER person_person_id_trg BEFORE
    INSERT ON person
    FOR EACH ROW
    WHEN ( new.person_id IS NULL )
BEGIN
    :new.person_id := person_person_id_seq.nextval;
END;
/

CREATE SEQUENCE residence_residence_id_seq START WITH 1 NOCACHE ORDER;

CREATE OR REPLACE TRIGGER residence_residence_id_trg BEFORE
    INSERT ON residence
    FOR EACH ROW
    WHEN ( new.residence_id IS NULL )
BEGIN
    :new.residence_id := residence_residence_id_seq.nextval;
END;
/

CREATE SEQUENCE school_school_id_seq START WITH 1 NOCACHE ORDER;

CREATE OR REPLACE TRIGGER school_school_id_trg BEFORE
    INSERT ON school
    FOR EACH ROW
    WHEN ( new.school_id IS NULL )
BEGIN
    :new.school_id := school_school_id_seq.nextval;
END;
/

CREATE SEQUENCE school_attendance_attendance_i START WITH 1 NOCACHE ORDER;

CREATE OR REPLACE TRIGGER school_attendance_attendance_i BEFORE
    INSERT ON school_attendance
    FOR EACH ROW
    WHEN ( new.attendance_id IS NULL )
BEGIN
    :new.attendance_id := school_attendance_attendance_i.nextval;
END;
/



-- Oracle SQL Developer Data Modeler Summary Report: 
-- 
-- CREATE TABLE                            12
-- CREATE INDEX                             2
-- ALTER TABLE                             32
-- CREATE VIEW                              0
-- ALTER VIEW                               0
-- CREATE PACKAGE                           0
-- CREATE PACKAGE BODY                      0
-- CREATE PROCEDURE                         0
-- CREATE FUNCTION                          0
-- CREATE TRIGGER                           7
-- ALTER TRIGGER                            0
-- CREATE COLLECTION TYPE                   0
-- CREATE STRUCTURED TYPE                   0
-- CREATE STRUCTURED TYPE BODY              0
-- CREATE CLUSTER                           0
-- CREATE CONTEXT                           0
-- CREATE DATABASE                          0
-- CREATE DIMENSION                         0
-- CREATE DIRECTORY                         0
-- CREATE DISK GROUP                        0
-- CREATE ROLE                              0
-- CREATE ROLLBACK SEGMENT                  0
-- CREATE SEQUENCE                          5
-- CREATE MATERIALIZED VIEW                 0
-- CREATE MATERIALIZED VIEW LOG             0
-- CREATE SYNONYM                           0
-- CREATE TABLESPACE                        0
-- CREATE USER                              0
-- 
-- DROP TABLESPACE                          0
-- DROP DATABASE                            0
-- 
-- REDACTION POLICY                         0
-- 
-- ORDS DROP SCHEMA                         0
-- ORDS ENABLE SCHEMA                       0
-- ORDS ENABLE OBJECT                       0
-- 
-- ERRORS                                   0
-- WARNINGS                                 0

