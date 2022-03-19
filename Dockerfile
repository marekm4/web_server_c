FROM debian:bullseye-slim as builder
RUN apt update && apt install gcc -y
COPY main.c main.c
RUN gcc -O3 -static -o main main.c

FROM scratch
COPY --from=builder main main
ENTRYPOINT ["/main"]
