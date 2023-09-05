%% PROJETO LABORATÓRIO INTEGRADOR

% ToolBoxes interessantes para uso
% ident
% ControlSystemDesigner

% A FAZER
% Mudar a frequência de amostragem para  100 Hz
% Coletar somente a descida de temperatura
% O motor começa a girar (vence seu torque) com PWM de 50. Assim que está em movimento, o limite para parar de girar é de 15

%% Análise da resposta do sistema a entrada definida
% Amostrado em 20Hz, com 4000 amostras

clear all
close all
clc

dados = importdata("Sistema2.txt");
Date = dados.textdata(:,1);          % Data
time = dados.data(:,1);          % Segundos
Temp = dados.data(:,2);          % Temperatura
pwm = dados.data(:,3);           % PWM

subplot(3,1,1);
plot(time,Temp);               
title("Temperatura")

subplot(3,1,2);
plot(time,pwm);
title("PWM")

subplot(3,1,3);
plot(time,Temp,time,pwm);
title("Temperatura e PWM")

%% ANÁLISES

s = tf('s')
model = idss(model_tf4);
G = ss(model.A,model.B,model.C,model.D);
[n,d] = ss2tf(G.A,G.B,G.C,G.D);
H = tf(n,d);
[A,B,C,D] = tf2ss(n,d);

Hz = c2d(G,0.05);
G = Hz;

T = feedback(G,1);

time = 0:0.05:200;
u = 50*ones(1,4001);
lsim(G,u,time,x0_tf4)
step(T)

%% Método pareamento de coeficientes
UP = 0.2;
Ts = 25;

% Pareamento de coeficientes
zeta = -log(UP)/(sqrt((pi^2)+(log(UP))^2));
wn = 4/(zeta*Ts);
r = exp(-zeta*wn*0.05);
teta = wn*0.05*sqrt(1-zeta^2);

Tsd = -4*0.05/log(r);
zetad = -log(r)/(sqrt((log(r))^2) + teta^2);
wnd = (1/0.05)*sqrt((log(r)^2) + teta^2);

p1 = complex(-zeta*wn, wn*sqrt(1-zeta^2));
p2 = complex(-zeta*wn, -wn*sqrt(1-zeta^2));
p3 = zero(G);
p4 = -20*zeta*wn;

sd = [p1 p2 p3 p4];

    % Controle integral
Ab = [G.A zeros(3,1); -G.C 0];
Bb = [G.B ; 0];

kb = place(Ab,Bb,sd);
K = kb(1:3);
Ki = -kb(4);

    % Malha fechada com ganhos
Am = [G.A-G.B*K, G.B*Ki; -G.C, 0];
Bm = [zeros(3,1); 1];
Cm = [G.C, 0];
Dm = 0;

Gss = ss(Am,Bm,Cm,Dm);
model = idss(Gss);