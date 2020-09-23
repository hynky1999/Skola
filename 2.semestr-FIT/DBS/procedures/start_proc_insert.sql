prompt #-----------------------#
prompt #- Smazat vsechna data -#
prompt #-----------------------#

exec VYMAZ_DATA_VSECH_TABULEK;

prompt #----------------------#
prompt #- Vypnout cizi klice -#
prompt #----------------------#

exec VYPNI_CIZI_KLICE;

prompt #---------------#
prompt #- Zmen datum  -#
prompt #---------------#

alter session set nls_date_format='mm/dd/yyyy';

prompt #---------------#
prompt #- Vlozit data -#
prompt #---------------#

