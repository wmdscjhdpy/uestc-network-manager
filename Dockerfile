#build image, not use in final image
FROM alpine:3.16 as builder
WORKDIR /etc/uestc_networkmanager/
COPY ./ /etc/uestc_networkmanager/
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

WORKDIR /etc/uestc_networkmanager/
RUN apk update && apk add openssl libgcc libstdc++ binutils
COPY --from=builder /etc/uestc_networkmanager/uestcnetwork .
#Create configure file
RUN echo -e "username=${username}\npassword=${password}\ninterval=${interval}\nac_id=${ac_id}\nkEncVer=${kEncVer}\nkDomain=${kDomain}">user.conf
ENTRYPOINT ["/etc/uestc_networkmanager/uestcnetwork"]