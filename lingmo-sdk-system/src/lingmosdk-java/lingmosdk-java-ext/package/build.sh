#!/usr/bin/env sh
cd ../lingmosdk-java/
mvn clean install -DskipTests=true --settings ../package/settings.xml
