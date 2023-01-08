FROM debian
RUN apt-get update && apt-get install -y build-essential
COPY main.c main.c
RUN make main
CMD ["./main"]
