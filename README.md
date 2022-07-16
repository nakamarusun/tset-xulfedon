### xulfedoN internship assesment

###### Now with database caching!
![demo](/images/demo.gif)

This assignment uses these libraries:
- libasyik
- json by nlohmann
- Boost
- SOCI

## How to run
1. To run you need to have [docker](https://www.docker.com/products/docker-desktop/) installed

2. Clone this repository
```bash
git clone https://github.com/nakamarusun/tset-xulfedon.git
cd tset-xulfedon
```

3. Build the docker image
```bash
docker build --tag xulfedon-tset:0.1 .
```

4. Create a docker volume to store the sqlite database
```bash
docker volume create covid-api-db
```

5. Run the container with the docker volume attached to it.
```bash
docker run -d -e DB_LOC="/var/docker_vol/covid.db" --mount type=volume,target=/var/docker_vol xulfedon-tset:0.1
```

## Features
- [x] Database data caching. since historical COVID-19 (usually) does not
change, there is little reason to refetch from the API everytime someone hits
our API. Therefore, we store it in a SQLite database.
- [x] Invalidate database entry. If we would like to, we can refetch the data
in the database by an additional parameter.

## Potential issues and limitations
- When getting data for month, or year, first we fetch the data,
Then we insert it to the database. After that, the database is queried and
we send the data to the user. This process can be faster if we fetch the data,
then send the data immediately to the user while inserting the data to the db
asynchronously using fibers.

## Future Ideations
- [ ] Middleware for HTTP servers
- [ ] Last modified header from covid api