
# PROGRAMMING ASSIGNMENT 2 
# ECEN 5273_PA2 Network Systems

#### To create a HTTP-based web server that handles multiple simultaneous requests from users. 





## Notes

I have implemented Error Handling, Multithreading to handle simultaneous connections to the web-server, GET request parsing and pipelining to an extent. POST requests have not been added.


## Authors

- [@saiabhishek28](https://github.com/saiabhishek28)




## Deployment

To deploy this project run

```bash
  gcc server.c -o server
  ./server <port_number>
```
## Example:
To run the HTTP server on port 8080 :
```bash
.\server 8080
```


