/* this is a REXX script */
parse value date('S') with 1 year 5 month 7 day
month = strip(month,'L',0)
day   = strip(day,'L',0)
vendor = 'Lars Erdmann'
vermajor = 1
verminor = 1

rc = value('VERMAJOR',vermajor,'OS2ENVIRONMENT')
rc = value('VERMINOR',verminor,'OS2ENVIRONMENT')
rc = value('DAY',day,'OS2ENVIRONMENT')
rc = value('MONTH',month,'OS2ENVIRONMENT')
rc = value('YEAR',year,'OS2ENVIRONMENT')
rc = value('VENDOR',vendor,'OS2ENVIRONMENT')
