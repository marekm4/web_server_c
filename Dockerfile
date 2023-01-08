FROM gcc as builder
COPY main.c main.c
RUN gcc -Os -static -o main main.c

FROM scratch
COPY --from=builder main main
CMD ["/main"]
