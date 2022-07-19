create table vessel_info (
  vessel_id INTEGER PRIMARY KEY,
  name char(30),
  client char(30),
  country char(30),
  field char(30)
  );

create table vessel_position (
  position_id INTEGER PRIMARY KEY,
  vessel_id INTEGER not NULL,
  position char(30),
  FOREIGN KEY(vessel_id) REFERENCES vessel_info(vessel_id)
  );

create table sensor_info (
  sensor_id INTEGER PRIMARY KEY,
  position_id INTEGER not NULL,
  type char(30),
  FOREIGN KEY(position_id) REFERENCES vessel_position(position_id)
  );

create table sg_rawdata (
  sg_rd_id DATETIME PRIMARY KEY,
  sensor_id INTEGER not NULL,
  point_1 REAL,
  point_2 REAL,
  point_3 REAL,
  point_x REAL,
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id)
  );

create table ecasr_data (
  ecasr_id INTEGER PRIMARY KEY,
  sensor_id INTEGER not NULL,
  sg_rd_id INTEGER not NULL,
  cycle_no INTEGER,
  ecasr REAL,
  difference decimal(5,2),
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id),
  FOREIGN KEY(sg_rd_id) REFERENCES sg_rawdata(sg_rd_id)
  );

create table conversion_matrix (
  cm_row_id INTEGER PRIMARY KEY,
  column_1 REAL,
  column_2 REAL,
  column_3 REAL
  );

create table pre_multiplication (
  pm_row_id INTEGER PRIMARY KEY,
  pre_multi REAL
  );

create table hull_girder_loads (
  hgl_id INTEGER PRIMARY KEY,
  sensor_id INTEGER not NULL,
  sg_rd_id DATETIME not NULL,
  ecasr_id INTEGER not NULL,
  dl_vbm REAL,
  dl_hbm REAL,
  dl_tbm REAL,
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id),
  FOREIGN KEY(sg_rd_id) REFERENCES sg_rawdata(sg_rd_id),
  FOREIGN KEY(ecasr_id) REFERENCES ecasr_data(ecasr_id)
  );

create table pd_coefficients (
  pd_coef_id INTEGER PRIMARY KEY,
  coef_0 REAL,
  coef_1 REAL,
  coef_2 REAL,
  coef_3 REAL,
  coef_4 REAL
  );

create table pd_fatigue_damage (
  pd_fd_id INTEGER PRIMARY KEY,
  sensor_id INTEGER not NULL,
  sg_rd_id DATETIME not NULL,
  ecasr_id INTEGER not NULL,
  hgl_id INTEGER not NULL,
  pd_fd REAL,
  pd_fd_cum REAL,
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id),
  FOREIGN KEY(sg_rd_id) REFERENCES sg_rawdata(sg_rd_id),
  FOREIGN KEY(ecasr_id) REFERENCES ecasr_data(ecasr_id),
  FOREIGN KEY(hgl_id) REFERENCES hull_girder_loads(hgl_id)
  );

create table umcd_coefficients (
  umcd_coef_id INTEGER PRIMARY KEY,
  coef_0 REAL,
  coef_1 REAL,
  coef_2 REAL
  );

create table unmonitored_critical_details (
  umcd_id INTEGER PRIMARY KEY,
  sensor_id INTEGER not NULL,
  sg_rd_id DATETIME not NULL,
  ecasr_id INTEGER not NULL,
  hgl_id INTEGER not NULL,
  ecasr_umcd REAL,
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id),
  FOREIGN KEY(sg_rd_id) REFERENCES sg_rawdata(sg_rd_id),
  FOREIGN KEY(ecasr_id) REFERENCES ecasr_data(ecasr_id),
  FOREIGN KEY(hgl_id) REFERENCES hull_girder_loads(hgl_id)
  );

create table mcd_coefficients (
  mcd_coef_id INTEGER PRIMARY KEY,
  coef_0 REAL,
  coef_1 REAL,
  coef_2 REAL
  );

create table monitored_critical_details (
  mcd_id INTEGER PRIMARY KEY,
  sensor_id INTEGER not NULL,
  sg_rd_id DATETIME not NULL,
  ecasr_id INTEGER not NULL,
  hgl_id INTEGER not NULL,
  ecasr_mcd REAL,
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id),
  FOREIGN KEY(sg_rd_id) REFERENCES sg_rawdata(sg_rd_id),
  FOREIGN KEY(ecasr_id) REFERENCES ecasr_data(ecasr_id),
  FOREIGN KEY(hgl_id) REFERENCES hull_girder_loads(hgl_id)
  );

create table snc_constants (
  snc_con_id INTEGER PRIMARY KEY,
  a REAL,
  m REAL,
  t REAL,
  t_ref REAL,
  k REAL
  );

create table snc_fatigue_damage (
  snc_fd_id INTEGER PRIMARY KEY,
  sensor_id INTEGER not NULL,
  sg_rd_id DATETIME not NULL,
  ecasr_id INTEGER not NULL,
  hgl_id INTEGER not NULL,
  N_total REAL,
  snc_fd REAL,
  snc_fd_cum REAL,
  FOREIGN KEY(sensor_id) REFERENCES sensor_info(sensor_id),
  FOREIGN KEY(sg_rd_id) REFERENCES sg_rawdata(sg_rd_id),
  FOREIGN KEY(ecasr_id) REFERENCES ecasr_data(ecasr_id),
  FOREIGN KEY(hgl_id) REFERENCES hull_girder_loads(hgl_id)
  );
