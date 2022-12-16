#build image, not use in final image
FROM ubuntu as builder
WORKDIR /uestc_networkmanager/
COPY ./ /uestc_networkmanager/
RUN apt-get update && apt install openssl gcc g++ make libffi-dev openssl-dev libtool cmake
RUN cmake . && make

#main image
FROM ubuntu
LABEL org.opencontainers.image.authors="wmdscjhdpy@gmail.com"

#Set envirement
ENV username=enteryournumber
ENV password=yourpassword
ENV interval=30
ENV ac_id=1
ENV kEncVer=srun_bx1
ENV kDomain=@dx-uestc

WORKDIR /uestc_networkmanager/
RUN apt-get update && apt-get install openssl libgcc libstdc++ binutils
COPY --from=builder /uestc_networkmanager/uestcnetwork .
VOLUME /etc/uestc_networkmanager/
#Create configure file
#RUN echo -e "username=${username}\npassword=${password}\ninterval=${interval}\nac_id=${ac_id}\nkEncVer=${kEncVer}\nkDomain=${kDomain}">user.conf
ENTRYPOINT ["/uestc_networkmanager/uestcnetwork"]