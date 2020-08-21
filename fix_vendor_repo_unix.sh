#!/usr/bin/env bash

# Reset changes if in git repository
if [[ -d ".git" ]]; then
    echo "in git repository"
    cd vendor/drogon && git reset --hard HEAD && cd ../../
    cd vendor/sqlpp11_connector_mysql && git reset --hard HEAD && cd ../../
else
    echo "NOT in git repository"
fi

# "Fix" drogon C++20 lambda capture errors
sed -i 's/\[=\]/\[this\]/' vendor/drogon/lib/inc/drogon/CacheMap.h
sed -i 's/cb = \[this\]/cb = \[key,this\]/' vendor/drogon/lib/inc/drogon/CacheMap.h
sed -i 's/CallbackEntry>(\[this\]/CallbackEntry>(\[delay,entryPtr,i,t,this\]/' vendor/drogon/lib/inc/drogon/CacheMap.h

# "Fix" sqlpp11 connector mysql unused variable error
sed -i '50,61s/ t, mysql/, mysql/' vendor/sqlpp11_connector_mysql/include/sqlpp11/mysql/serializer.h
