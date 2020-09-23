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
