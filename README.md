### xulfedoN internship assesment
COVID-19 HTTP API Server with a little twist

[image]

## API list

## How to run

## Features
- [ ] Database data caching. since historical COVID-19 (usually) does not
change, there is little reason to refetch from the API everytime someone hits
our API. Therefore, we store it in a SQLite database.
- [ ] Invalidate database entry. If we would like to, we can refetch the data
in the database by an additional parameter.

## Potential issues and limitations
- When getting data for month, or year, first we fetch the data,
Then we insert it to the database. After that, the database is queried and
we send the data to the user. This process can be faster if we fetch the data,
then send the data immediately to the user while inserting the data to the db
asynchronously using fibers.

## Future Ideations
- [ ] TODO: Middleware for HTTP servers
- [ ] TODO: Last modified header from covid api