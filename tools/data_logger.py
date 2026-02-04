#!/usr/bin/env python3
"""
Data Logger - Lee datos del puerto COM y los guarda en CSV
ESP32 Hardware Timer - Captura de interrupciones y valores analógicos
"""

import serial
import csv
from datetime import datetime
import argparse


class DataLogger:
    """Clase para leer datos del puerto COM y guardarlos en CSV"""
    
    def __init__(self, port, baudrate=115200, csv_file='../data/registro.csv'):
        """
        Inicializa el logger de datos
        
        Args:
            port (str): Puerto COM (ej: 'COM3' en Windows)
            baudrate (int): Velocidad del puerto (default: 115200)
            csv_file (str): Ruta del archivo CSV de salida
        """
        self.port = port  # Puerto serial a usar
        self.baudrate = baudrate  # Velocidad de comunicación
        self.csv_file = csv_file  # Ruta del archivo CSV
        self.serial_conn = None  # Conexión serial
        self.csv_writer = None  # Escritor CSV
        self.csv_file_handle = None  # Archivo CSV abierto
        self.total_lines = 0  # Contador de líneas grabadas
    
    def connect(self):
        """Establece conexión con el puerto COM"""
        try:
            self.serial_conn = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=1
            )
            print(f"✓ Conectado a {self.port} @ {self.baudrate} baud")
            return True
        except serial.SerialException as e:
            print(f"✗ Error al conectar a {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Cierra la conexión con el puerto COM"""
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
            print("✓ Desconectado del puerto COM")
    
    def _init_csv_writer(self):
        """Inicializa el archivo CSV con encabezados"""
        try:
            self.csv_file_handle = open(self.csv_file, 'w+', newline='', encoding='utf-8')
            self.csv_writer = csv.writer(self.csv_file_handle)
            
            # Escribir encabezados
            self.csv_writer.writerow(['timestamp', 'interrupt_count', 'adc_value'])
            self.csv_file_handle.flush()
            print(f"✓ Archivo CSV creado: {self.csv_file}")
            return True
        except IOError as e:
            print(f"✗ Error al crear archivo CSV: {e}")
            return False
    
    def parse_data(self, line):
        """
        Parsea una línea de datos del ESP32
        Formato esperado: "100,1234" (interrupt_count,adc_value)
        
        Returns:
            tuple: (interrupt_count, adc_value) o None si hay error
        """
        try:
            parts = line.split(',')
            if len(parts) != 2:
                return None
            
            interrupt_count = int(parts[0].strip())
            adc_value = int(parts[1].strip())
            
            return (interrupt_count, adc_value)
        except (ValueError, IndexError):
            return None
    
    def start_logging(self):
        """Inicia la captura de datos"""
        if not self.connect():
            return False
        
        if not self._init_csv_writer():
            self.disconnect()
            return False
        
        print("\n" + "="*60)
        print("CAPTURA DE DATOS INICIADA")
        print("="*60)
        print("Presiona Ctrl+C para detener\n")
        
        try:
            while True:
                # Leer línea del puerto serial
                if self.serial_conn.in_waiting:
                    try:
                        line = self.serial_conn.readline().decode('utf-8').strip()
                        
                        # Ignorar líneas vacías
                        if not line:
                            continue
                        
                        # Parsear datos
                        data = self.parse_data(line)
                        if data:
                            interrupt_count, adc_value = data
                            timestamp = datetime.now().isoformat()
                            
                            # Escribir en CSV
                            self.csv_writer.writerow([
                                timestamp,
                                interrupt_count,
                                adc_value
                            ])
                            
                            # Guardar cada 10 líneas
                            self.total_lines += 1
                            if self.total_lines % 10 == 0:
                                self.csv_file_handle.flush()
                                print(f"[{timestamp}] Interrupts: {interrupt_count:6d} | ADC: {adc_value:4d}")
                    
                    except UnicodeDecodeError:
                        continue
        
        except KeyboardInterrupt:
            print("\n\n⚠ Captura interrumpida por el usuario")
        
        finally:
            self._finalize_logging()
        
        return True
    
    def _finalize_logging(self):
        """Finaliza la captura y cierra archivos"""
        if self.csv_file_handle:
            self.csv_file_handle.close()
        
        self.disconnect()
        
        print("\n" + "="*60)
        print("CAPTURA FINALIZADA")
        print("="*60)
        print(f"✓ Total de líneas registradas: {self.total_lines}")
        print(f"✓ Archivo guardado: {self.csv_file}")
        print("="*60)


def main():
    """Función principal"""
    parser = argparse.ArgumentParser(
        description='Data Logger - Lee datos del puerto COM y los guarda en CSV'
    )
    
    parser.add_argument('-p', '--port', default='COM4', help='Puerto COM (ej: COM3)')
    parser.add_argument('-b', '--baudrate', type=int, default=921600, help='Velocidad en baud (default: 921600)')
    parser.add_argument('-o', '--output', default=r'C:\sensor-intelligence-lab\data\registro.csv', help='Archivo CSV de salida')
    
    args = parser.parse_args()
    
    # Crear logger y iniciar captura
    logger = DataLogger(
        port=args.port,
        baudrate=args.baudrate,
        csv_file=args.output
    )
    
    logger.start_logging()


if __name__ == '__main__':
    main()