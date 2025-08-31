FROM gcc AS builder
COPY main.c main.c
RUN gcc -Os -static -o main main.c

FROM scratch
COPY --from=builder main main
COPY index.html index.html
CMD ["/main"]
