#build image, not use in final image
FROM ubuntu:18.04 as builder
WORKDIR /uestc_networkmanager/
ENV TZ=Asia/Chongqing
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
COPY ./ /uestc_networkmanager/
RUN apt-get update && apt-get install -y libssl-dev build-essential make cmake
RUN cmake . && make

#main image
FROM ubuntu:18.04
LABEL org.opencontainers.image.authors="wmdscjhdpy@gmail.com"

#Set envirement
ENV username=enteryournumber
ENV password=yourpassword
ENV interval=30
ENV ac_id=1
ENV kEncVer=srun_bx1
ENV kDomain=@dx-uestc

WORKDIR /uestc_networkmanager/
# ENV TZ=Asia/Chongqing
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && apt-get install -y libssl-dev 
COPY --from=builder /uestc_networkmanager/uestcnetwork .
VOLUME /etc/uestc_networkmanager/
#Create configure file
#RUN echo -e "username=${username}\npassword=${password}\ninterval=${interval}\nac_id=${ac_id}\nkEncVer=${kEncVer}\nkDomain=${kDomain}">user.conf
ENTRYPOINT ["/uestc_networkmanager/uestcnetwork"]
