FROM debian:bullseye-slim as builder
RUN apt update && apt install gcc -y
COPY main.c main.c
RUN gcc -static -o main main.c

FROM scratch
ENV PORT=8080
COPY --from=builder main main
CMD ["/main"]
