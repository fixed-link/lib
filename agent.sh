#!/bin/sh

eval $(ssh-agent)
ssh-add ~/.ssh/xxx_rsa

# config for ssh to use different key

#host www.example.com
#        identityfile ~/.ssh/xxx_rsa

#host *
#        identityfile ~/.ssh/id_rsa
