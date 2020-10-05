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
-- Generated by Oracle SQL Developer Data Modeler 19.2.0.182.1216
--   at:        2020-04-18 12:31:04 CEST
--   site:      Oracle Database 11g
--   type:      Oracle Database 11g



CREATE TABLE bedding (
    bedding_ean   NUMBER NOT NULL,
    pattern       VARCHAR2(25 CHAR),
    washed        CHAR(1) NOT NULL,
    person_id     NUMBER
);

CREATE UNIQUE INDEX bedding__idx ON
    bedding (
        person_id
    ASC );

ALTER TABLE bedding ADD CONSTRAINT bedding_pk PRIMARY KEY ( bedding_ean );

CREATE TABLE bike (
    manufacturer       VARCHAR2(30 CHAR) NOT NULL,
    manufacture_date   DATE,
    bike_size          VARCHAR2(5 CHAR),
    type               VARCHAR2(15 CHAR) NOT NULL,
    bike_id            NUMBER NOT NULL,
    room_id            NUMBER,
    floor_number       NUMBER,
    mark               CHAR(3 BYTE)
);

ALTER TABLE bike ADD CONSTRAINT bike_pk PRIMARY KEY ( bike_id );

CREATE TABLE bike_storage (
    floor_number   NUMBER NOT NULL,
    room_id        NUMBER NOT NULL,
    mark           CHAR(3 BYTE) NOT NULL,
    capacity       NUMBER NOT NULL,
    person_id      NUMBER NOT NULL
);

ALTER TABLE bike_storage
    ADD CONSTRAINT bike_storage_pk PRIMARY KEY ( room_id,
                                                 floor_number,
                                                 mark );

CREATE TABLE building (
    mark     CHAR(3 BYTE) NOT NULL,
    height   NUMBER
);

ALTER TABLE building ADD CONSTRAINT building_pk PRIMARY KEY ( mark );

CREATE TABLE floor (
    floor_number   NUMBER NOT NULL,
    mark           CHAR(3 BYTE) NOT NULL
);

ALTER TABLE floor ADD CONSTRAINT floor_pk PRIMARY KEY ( floor_number,
                                                        mark );

CREATE TABLE furniture (
    material        VARCHAR2(25 CHAR),
    condition       VARCHAR2(25 CHAR),
    furniture_ean   NUMBER NOT NULL,
    type            VARCHAR2(25 CHAR) NOT NULL,
    room_id         NUMBER,
    floor_number    NUMBER,
    mark            CHAR(3 BYTE)
);

ALTER TABLE furniture ADD CONSTRAINT furniture_pk PRIMARY KEY ( furniture_ean );

CREATE TABLE person (
    surname       VARCHAR2(30 CHAR) NOT NULL,
    age           NUMBER,
    firstname     VARCHAR2(30 CHAR) NOT NULL,
    gender        VARCHAR2(6 CHAR),
    person_id     NUMBER NOT NULL,
    bedding_ean   NUMBER,
    bike_id       NUMBER
);

ALTER TABLE person ADD CHECK ( age BETWEEN 18 AND 150 );

ALTER TABLE person
    ADD CHECK ( gender IN (
        'Female',
        'Male'
    ) );

CREATE UNIQUE INDEX person__idx ON
    person (
        bedding_ean
    ASC );

ALTER TABLE person ADD CONSTRAINT person_pk PRIMARY KEY ( person_id );

CREATE TABLE residence (
    residence_start   DATE NOT NULL,
    price_month       NUMBER NOT NULL,
    person_id         NUMBER NOT NULL,
    residence_id      NUMBER NOT NULL,
    residence_end     DATE,
    room_id           NUMBER NOT NULL,
    floor_number      NUMBER NOT NULL,
    mark              CHAR(3 BYTE) NOT NULL
);

ALTER TABLE residence ADD CONSTRAINT residence_pk PRIMARY KEY ( residence_id );

CREATE TABLE residence_room (
    floor_number   NUMBER NOT NULL,
    room_id        NUMBER NOT NULL,
    mark           CHAR(3 BYTE) NOT NULL,
    capacity       NUMBER NOT NULL,
    condition      VARCHAR2(25 CHAR)
);

ALTER TABLE residence_room ADD CHECK ( capacity BETWEEN 0 AND 4 );

ALTER TABLE residence_room
    ADD CONSTRAINT residence_room_pk PRIMARY KEY ( room_id,
                                                   floor_number,
                                                   mark );

CREATE TABLE room (
    floor_number   NUMBER NOT NULL,
    room_id        NUMBER NOT NULL,
    mark           CHAR(3 BYTE) NOT NULL,
    area           NUMBER NOT NULL,
    room_type      VARCHAR2(14) NOT NULL
);

ALTER TABLE room
    ADD CONSTRAINT ch_inh_room CHECK ( room_type IN (
        'Bike_storage',
        'Residence_Room'
    ) );

ALTER TABLE room
    ADD CONSTRAINT room_pk PRIMARY KEY ( room_id,
                                         floor_number,
                                         mark );

CREATE TABLE school (
    city        VARCHAR2(50 CHAR) NOT NULL,
    street      VARCHAR2(50 CHAR) NOT NULL,
    postcode    VARCHAR2(25 CHAR) NOT NULL,
    name        VARCHAR2(70 CHAR) NOT NULL,
    major       VARCHAR2(50 CHAR),
    school_id   NUMBER NOT NULL
);

ALTER TABLE school ADD CONSTRAINT school_pk PRIMARY KEY ( school_id );

CREATE TABLE school_attendance (
    degree                VARCHAR2(25 CHAR),
    grade_point_average   NUMBER,
    attendance_id         NUMBER NOT NULL,
    person_id             NUMBER NOT NULL,
    school_id             NUMBER NOT NULL,
    attendance_start      DATE NOT NULL,
    attendance_end        DATE
);

ALTER TABLE school_attendance ADD CONSTRAINT school_attendance_pk PRIMARY KEY ( attendance_id );

ALTER TABLE bedding
    ADD CONSTRAINT bedding_person_fk FOREIGN KEY ( person_id )
        REFERENCES person ( person_id );

ALTER TABLE bike
    ADD CONSTRAINT bike_bike_storage_fk FOREIGN KEY ( room_id,
                                                      floor_number,
                                                      mark )
        REFERENCES bike_storage ( room_id,
                                  floor_number,
                                  mark );

ALTER TABLE bike_storage
    ADD CONSTRAINT bike_storage_person_fk FOREIGN KEY ( person_id )
        REFERENCES person ( person_id );

ALTER TABLE bike_storage
    ADD CONSTRAINT bike_storage_room_fk FOREIGN KEY ( room_id,
                                                      floor_number,
                                                      mark )
        REFERENCES room ( room_id,
                          floor_number,
                          mark );

ALTER TABLE floor
    ADD CONSTRAINT floor_building_fk FOREIGN KEY ( mark )
        REFERENCES building ( mark );

ALTER TABLE furniture
    ADD CONSTRAINT furniture_room_fk FOREIGN KEY ( room_id,
                                                   floor_number,
                                                   mark )
        REFERENCES room ( room_id,
                          floor_number,
                          mark );

ALTER TABLE person
    ADD CONSTRAINT person_bedding_fk FOREIGN KEY ( bedding_ean )
        REFERENCES bedding ( bedding_ean );

ALTER TABLE person
    ADD CONSTRAINT person_bike_fk FOREIGN KEY ( bike_id )
        REFERENCES bike ( bike_id );

ALTER TABLE residence
    ADD CONSTRAINT residence_person_fk FOREIGN KEY ( person_id )
        REFERENCES person ( person_id );

ALTER TABLE residence
    ADD CONSTRAINT residence_residence_room_fk FOREIGN KEY ( room_id,
                                                             floor_number,
                                                             mark )
        REFERENCES residence_room ( room_id,
                                    floor_number,
                                    mark );

ALTER TABLE residence_room
    ADD CONSTRAINT residence_room_room_fk FOREIGN KEY ( room_id,
                                                        floor_number,
                                                        mark )
        REFERENCES room ( room_id,
                          floor_number,
                          mark );

ALTER TABLE room
    ADD CONSTRAINT room_floor_fk FOREIGN KEY ( floor_number,
                                               mark )
        REFERENCES floor ( floor_number,
                           mark );

ALTER TABLE school_attendance
    ADD CONSTRAINT school_attendance_person_fk FOREIGN KEY ( person_id )
        REFERENCES person ( person_id );

ALTER TABLE school_attendance
    ADD CONSTRAINT school_attendance_school_fk FOREIGN KEY ( school_id )
        REFERENCES school ( school_id );

CREATE OR REPLACE TRIGGER arc_fkarc_5_bike_storage BEFORE
    INSERT OR UPDATE OF room_id, floor_number, mark ON bike_storage
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
        AND a.floor_number = :new.floor_number
        AND a.mark = :new.mark;

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

CREATE OR REPLACE TRIGGER arc_fkarc_5_residence_room BEFORE
    INSERT OR UPDATE OF room_id, floor_number, mark ON residence_room
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
        AND a.floor_number = :new.floor_number
        AND a.mark = :new.mark;

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
-- ALTER TABLE                             30
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