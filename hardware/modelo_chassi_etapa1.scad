// ============================================================
// CARRINHO MAKER LAB - Modelo de REFERÊNCIA (v3)
// Chassi pronto (kit 4WD acrílico duplo) + Arduino + L298N + Motores + Sensor
// ATENÇÃO: dimensões do chassi são baseadas na especificação do anúncio
// (256 x 150 x 65mm). Furos exatos do kit físico podem variar levemente
// -- ajustar assim que o kit chegar e as posições reais forem conferidas.
// ============================================================
$fn = 48;

// --- PARÂMETROS GERAIS DO CHASSI (kit pronto, 2 níveis) ---
chassi_L = 256;
chassi_W = 150;
chassi_T = 3;
nivel_gap = 45;      // distância entre a placa inferior e a superior

// --- POSIÇÕES DOS MÓDULOS (nível superior) ---
arduino_x = 0;  arduino_y = 40;
l298_x = 0;     l298_y = -35;
bateria_x = 0;  bateria_y = -60;   // nível inferior
sensor_y = chassi_L/2 - 25;        // nível superior, frente

// --- DIMENSÕES DOS MÓDULOS ---
arduino_w = 68.6; arduino_d = 53.4; arduino_h = 12;
l298_w = 43;       l298_d = 43;      l298_h = 20;
bateria_w = 70;    bateria_d = 63;   bateria_h = 16;
sensor_w = 45;     sensor_d = 20;    sensor_h = 15;
motor_w = 22;      motor_d = 50;     motor_h = 18;
motor_can_len = 22;
roda_diam = 68;    roda_larg = 24;

module rounded_rect(w, l, r) {
    hull() {
        for (sx = [-1, 1]) for (sy = [-1, 1])
            translate([sx*(w/2 - r), sy*(l/2 - r)]) circle(r=r);
    }
}

// ============================================================
// PLACA (nível inferior ou superior) - genérica, com furos de referência
// ============================================================
module placa(z, furos=[]) {
    color("LightBlue", 0.55)
    translate([0,0,z])
    difference() {
        linear_extrude(height = chassi_T)
            rounded_rect(chassi_W, chassi_L, 10);
        for (p = furos)
            translate([p[0], p[1], -1]) cylinder(h=chassi_T+2, d=3.2);
    }
}

// furos de fixação dos motores (4 cantos, entre as placas)
motor_furos = [
    for (sx = [-1, 1], sy = [-1, 1], dx = [-10, 10])
        [sx * (chassi_W/2 - 20) + dx, sy * (chassi_L/2 - 45)]
];

// furos do Arduino (placa superior)
ard_dx = arduino_w - 8; ard_dy = arduino_d - 8;
arduino_furos = [
    for (sx=[-1,1], sy=[-1,1]) [arduino_x+sx*ard_dx/2, arduino_y+sy*ard_dy/2]
];

// furos da ponte H (placa superior)
l298_dx = l298_w - 6; l298_dy = l298_d - 6;
l298_furos = [
    for (sx=[-1,1], sy=[-1,1]) [l298_x+sx*l298_dx/2, l298_y+sy*l298_dy/2]
];

// ============================================================
// COMPONENTES (mesmos módulos detalhados da versão anterior)
// ============================================================
module rounded_box(w, d, h, r) {
    linear_extrude(height=h) offset(r=r) offset(delta=-r) square([w, d], center=true);
}

module arduino(z) {
    translate([arduino_x, arduino_y, z]) {
        color("RoyalBlue") rounded_box(arduino_w, arduino_d, arduino_h, 2);
        color("Silver") translate([-arduino_w/2 - 4, -8, 3]) cube([8, 16, 8]);
        color("Black") translate([-arduino_w/2 + 4, arduino_d/2 - 3, arduino_h - 1]) cube([arduino_w - 8, 3, 3]);
    }
}

module l298n(z) {
    translate([l298_x, l298_y, z]) {
        color("DarkOrange") rounded_box(l298_w, l298_d, l298_h - 8, 1.5);
        color("Silver")
        translate([-10, -l298_d/2 + 4, l298_h - 8])
            for (i = [0:5]) translate([i*3.5, 0, 0]) cube([2, l298_d - 8, 8]);
        color("DimGray") translate([-l298_w/2, -6, 0]) cube([6, 12, 10]);
    }
}

module bateria(z) {
    translate([bateria_x, bateria_y, z]) {
        color("MediumPurple") rounded_box(bateria_w, bateria_d, bateria_h, 3);
        color("Crimson") translate([bateria_w/2 - 8, 0, bateria_h - 1]) cylinder(h=4, d=6);
    }
}

module sensor(z) {
    translate([0, sensor_y, z]) {
        color("ForestGreen") rounded_box(sensor_w, sensor_d, 3, 1);
        color("Silver")
        for (sx = [-1, 1])
            translate([sx*12, sensor_d/2 - 2, 2]) cylinder(h=8, d=16);
    }
}

module motor(x, y, z, dir) {
    translate([x, y, z]) {
        color("Khaki") rotate([90,0,0]) translate([0,0,-motor_d/2])
            linear_extrude(height=motor_d) offset(r=1.5) offset(delta=-1.5) square([motor_w, motor_h], center=true);
        color("Gainsboro") translate([0, -dir*(motor_d/2 + motor_can_len/2), 0])
            rotate([90,0,0]) cylinder(h=motor_can_len, d=motor_h - 2, center=true);
        color("Silver") translate([0, dir*(motor_d/2 + 4), 0])
            rotate([90,0,0]) cylinder(h=8, d=4, center=true);
    }
}

module roda(x, y, z) {
    translate([x, y, z]) rotate([0, 90, 0]) {
        color("DimGray") difference() {
            cylinder(h=roda_larg, d=roda_diam, center=true);
            cylinder(h=roda_larg+1, d=roda_diam - 10, center=true);
        }
        color("Silver") cylinder(h=roda_larg - 4, d=roda_diam - 10, center=true);
        color("DarkSlateGray") cylinder(h=roda_larg + 0.5, d=10, center=true);
    }
}

// coluna espaçadora entre as duas placas
module coluna(x, y) {
    color("Gray") translate([x, y, chassi_T]) cylinder(h=nivel_gap, d=6);
}

// ============================================================
// MONTAGEM COMPLETA
// ============================================================
z_inferior = 0;
z_motor = chassi_T + motor_h/2 + 6;         // motores entre as placas
z_superior = chassi_T + nivel_gap;

// placa inferior (com furo de referência dos motores)
placa(z_inferior, motor_furos);
// placa superior (com furos de Arduino e L298N)
placa(z_superior, concat(arduino_furos, l298_furos));

// colunas espaçadoras nos 4 cantos
for (sx=[-1,1]) for (sy=[-1,1])
    coluna(sx*(chassi_W/2 - 10), sy*(chassi_L/2 - 10));

// bateria no nível inferior
bateria(z_inferior + chassi_T);

// Arduino, L298N e sensor no nível superior
arduino(z_superior + chassi_T);
l298n(z_superior + chassi_T);
sensor(z_superior + chassi_T);

// motores e rodas nos 4 cantos, entre as placas
for (sx = [-1, 1]) for (sy = [-1, 1]) {
    mx = sx * (chassi_W/2 + motor_w/2 + 6);
    my = sy * (chassi_L/2 - 75);
    motor(mx, my, z_motor, sx);

    rx = sx * (chassi_W/2 + motor_w + 6 + roda_larg/2 + 2);
    roda(rx, my, z_motor);
}
