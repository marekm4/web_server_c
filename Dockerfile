FROM debian as builder
RUN apt-get update && apt-get install gcc -y
COPY main.c main.c
RUN gcc -O3 -static -o main main.c

FROM scratch
ENV PORT=8080
COPY --from=builder main main
ENTRYPOINT ["/main"]
