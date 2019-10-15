FROM alpine:3.9 as build
RUN apk update \
    && apk add --virtual build-dependencies \
        build-base \
        gcc 

WORKDIR /bot
COPY . .
RUN make


FROM alpine:3.9
COPY --from=build /bot/bin/hernan /bin/
RUN apk update \
    && apk add --no-cache  \
        libstdc++
CMD ["bin/hernan"]
