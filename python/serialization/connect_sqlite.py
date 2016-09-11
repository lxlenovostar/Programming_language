#! /usr/bin/env python

import sqlite3 

conn = sqlite3.connect('inventory.db')

cursor = conn.execute("insert into inventory_operatingsystem (name, description) values ('Linux', '2.0.34 kernel');")
cursor.fetchall() 
conn.commit()
