## MySQL UDF: base64encode / base64decode

## Summary

It provides base64 encode/decode function for MySQL-5.1/5.5/5.6 as UDF.
It has based on base64_encode/base64_decode of PHP implementation.

## Compatibility

* MySQL-5.1
* MySQL-5.5
* MySQL-5.6

## Usage

```sql
SELECT base64encode('data,binary,text,...');
SELECT base64decode('b64strings');
INSERT INTO t1 (body) VALUES (base64encode('something'));
```

## Installation

build it.

```sh
$ git clone https://github.com/y-ken/mysql-udf-base64.git
$ cd mysql-udf-base64
$ gcc -Wall -fPIC -I/usr/local/include -shared base64.c -o base64.so
$ sudo install -m 755 base64.so `mysql_config --plugindir`
```

activate it.

```sql
mysql> CREATE FUNCTION base64encode RETURNS STRING SONAME 'base64.so';
mysql> CREATE FUNCTION base64decode RETURNS STRING SONAME 'base64.so';
```

use it.

```sql
mysql> CREATE TABLE t1 (body text);
mysql> INSERT INTO t1 (body) VALUES (base64encode('something'));
mysql> SELECT body, base64decode(body) from t1;
```

## Notes

Below is the original README.

```
this code based on PHP's base64.c
term is below.

Murakami Takeshi
takeshi@softagency.co.jp

--- changelog
2001-06-18

   * base64decode can handle LF,CR,'\'

  -- takeshi@softagency.co.jp 

2001-06-15

   * init.

  -- takeshi@softagency.co.jp 
```
