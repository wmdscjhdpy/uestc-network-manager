#build image, not use in final image
FROM alpine:3.16 as builder
WORKDIR /uestc_networkmanager/
COPY ./ /uestc_networkmanager/
RUN apk update && apk add openssl gcc g++ make libffi-dev openssl-dev libtool cmake
RUN cmake . && make

#main image
FROM alpine:3.16
LABEL org.opencontainers.image.authors="wmdscjhdpy@gmail.com"

#Set envirement
ENV username=enteryournumber
ENV password=yourpassword
ENV interval=30
ENV ac_id=1
ENV kEncVer=srun_bx1
ENV kDomain=@dx-uestc

WORKDIR /uestc_networkmanager/
RUN apk update && apk add openssl libgcc libstdc++ binutils
COPY --from=builder /uestc_networkmanager/uestcnetwork .
VOLUME /etc/uestc_networkmanager/
#Create configure file
#RUN echo -e "username=${username}\npassword=${password}\ninterval=${interval}\nac_id=${ac_id}\nkEncVer=${kEncVer}\nkDomain=${kDomain}">user.conf
ENTRYPOINT ["/uestc_networkmanager/uestcnetwork"]