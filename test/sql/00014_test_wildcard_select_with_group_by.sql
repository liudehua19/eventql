-- IMPORT customers FROM ./test/sql_testdata/testtbl2.csv
select * from (select city, customername from customers) group by city order by city;
