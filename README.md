## MySQL UDF: base64encode / base64decode

## Usage

```sql
SELECT base64encode('data,binary,text,...');
SELECT base64decode('b64strings');
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

## Notes

It is original README below.

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
