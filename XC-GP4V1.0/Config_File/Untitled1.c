int CalcAverageWaitingTime(int ElevatorCounter, int floor_index,Direction_Enm_TypeDef direction) //calculat the average waiting time 
{
  uint8_t FloorTime =3;    //!- Constant Value 3 Sec Elevator traveling time for single floor
  Elevator_TypeDef elevator_cpy;
  int SwitchMode = 0;
  int RegCall = 0;
  int i = 0;
  int TravelingTime = 0;                       //Total traveling time
  int TotalStopageTime = 0;                    //Total Stoppage Time
  int minimum;
  int maximum;
  elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_cpy, (Elevator_No_TypeDef)ElevatorCounter);
  //  for (int floor = (NUMBER_FLOOR-1); floor >= 0; floor--)
  //  {
  //    if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
  //    {
  //      maximum = floor;
  //      break;
  //    }
  //  }
  //  for (int floor = 0; floor < NUMBER_FLOOR; floor++)
  //  {
  //    if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
  //    {
  //      minimum = floor;
  //      break;
  //    }
  //  }
  switch ( direction)
  {
  case UP: /////////////////////////////////////////////////////////////// UP  /////////////////////////////////////////////////
    if ((all_requestsUP[floor_index].request.position > elevator_cpy.position)) 
      SwitchMode = 1;      /*! Car position is < Request Position*//*! Car is under Request */
    else
      SwitchMode = 2;       /*! Car position is > Request Position*/   /*! Car is above Request */
    switch (SwitchMode)
    {
    case 1:   
      RegCall = 0;
      if (elevator_cpy.direction == UP)/*! Car position is < Request Position*/
      {
        TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
        for (i =  elevator_cpy.position; i <  all_requestsUP[floor_index].request.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;           //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if (elevator_cpy.direction == DOWN) /*! Car position is < Request Position*/
      {
        minimum = elevator_cpy.position;
        for (int floor = 0; floor < NUMBER_FLOOR; floor++)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            minimum = floor;
            break;
          }
        }
        TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
        for (i =  minimum; i <  all_requestsUP[floor_index].request.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;           //clear Car Call
            RegCall++;
          }
        }
        for (i =  minimum; i <=  elevator_cpy.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
          {
            elevator_cpy.internal_request[i].active = 0;                            //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE)) /*! Car position is < Request Position*/
      {
        TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
        for (i =  elevator_cpy.position; i <  all_requestsUP[floor_index].request.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;           //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      break;
    case 2:
      RegCall = 0;
      if ((elevator_cpy.direction == UP)) /*! Car position is > Request Position*/
      {
        maximum = elevator_cpy.position;
        for (int floor = (NUMBER_FLOOR-1); floor >= 0; floor--)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            maximum = floor;
            break;
          }
        }
        minimum = all_requestsUP[floor_index].request.position;
        for (int floor = 0; floor < NUMBER_FLOOR; floor++)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            minimum = floor;
            break;
          }
        }
        if (minimum > all_requestsUP[floor_index].request.position)
        {
          TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsUP[floor_index].request.position)) * FloorTime;
          for (i =  elevator_cpy.position; i <=  maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          for (i =  all_requestsUP[floor_index].request.position; i <=  maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
        }
        else
        {
          TravelingTime = ((maximum - elevator_cpy.position) + (all_requestsUP[floor_index].request.position - minimum) + (maximum - minimum)) * FloorTime;
          for (i =  elevator_cpy.position; i <=  maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;           //clear Car Call
              RegCall++;
            }
          }
          for (i =  minimum; i <=  maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          for (i =  minimum; i <  all_requestsUP[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if (elevator_cpy.direction == DOWN)
      {
        minimum = all_requestsUP[floor_index].request.position;
        for (int floor = 0; floor < NUMBER_FLOOR; floor++)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            minimum = floor;
            break;
          }
        }
        TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
        for (i =  minimum; i <  all_requestsUP[floor_index].request.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || (elevator_cpy.external_requestUP[i].active != 0))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        for (i =  minimum; i <=  elevator_cpy.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || (elevator_cpy.external_requestDOWN[i].active != 0))////////Car and Landing DOWN
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
      {
        TravelingTime = (elevator_cpy.position - all_requestsUP[floor_index].request.position) * FloorTime;
        for (i =  elevator_cpy.position; i <  all_requestsUP[floor_index].request.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP Calls
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      break;
    }
    break;
  case DOWN: ///////////////////////////////////////////// Landing Call DOWN  //////////////////////////////////////////////
    if ((all_requestsDOWN[floor_index].request.position > elevator_cpy.position)) 
      SwitchMode = 1;      /*! Car position is < Request Position*//*! Car is under Request */
    else
      SwitchMode = 2;       /*! Car position is > Request Position*/   /*! Car is above Request */
    switch (SwitchMode)
    {
      /*! Car position is < Request Position*/
    case 1:
      RegCall = 0;
      if (elevator_cpy.direction == UP)
      {
        maximum=all_requestsDOWN[floor_index].request.position;
        for (int floor = (NUMBER_FLOOR-1); floor >= 0; floor--)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            maximum = floor;
            break;
          }
        }
        if(maximum<all_requestsDOWN[floor_index].request.position) maximum=all_requestsDOWN[floor_index].request.position;
        TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsDOWN[floor_index].request.position)) * FloorTime;
        for (i =  elevator_cpy.position; i <=  maximum; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        for (i =  all_requestsDOWN[floor_index].request.position; i <=  maximum; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[(TOTAL_REQ_NO - i)].active != 0)))////////Car and Landing DOWN
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if (elevator_cpy.direction == DOWN)
      {
        for (int floor = (NUMBER_FLOOR-1); floor >= 0; floor--)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            maximum = floor;
            break;
          }
        }
        minimum = elevator_cpy.position;
        for(int floor = 0; floor < NUMBER_FLOOR; floor++)
        {
          if((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            minimum = floor;
            break;
          }
        }
        if (all_requestsDOWN[floor_index].request.position >= maximum) maximum=all_requestsDOWN[floor_index].request.position; 
          TravelingTime = ((elevator_cpy.position - minimum) + (maximum - minimum) + (maximum - all_requestsDOWN[floor_index].request.position)) * FloorTime;
          for (i =  minimum; i <=  elevator_cpy.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          for (i =  minimum; i <=  maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
          for (i =  all_requestsDOWN[floor_index].request.position; i <=  maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              RegCall++;
            }
          }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
      {
        TravelingTime = (all_requestsDOWN[floor_index].request.position - elevator_cpy.position) * FloorTime;
        for (i =  elevator_cpy.position; i <  all_requestsDOWN[floor_index].request.position; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      break;
      /*! Car position is > Request Position*/
    case 2:
      RegCall = 0;
      if (elevator_cpy.direction == UP)
      { 
        for (int floor = (NUMBER_FLOOR-1); floor >= 0; floor--)
        {
          if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
          {
            maximum = floor;
            break;
          }
        }
        if(maximum<elevator_cpy.position) maximum=elevator_cpy.position;
        TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsDOWN[floor_index].request.position)) * FloorTime;
        for (i =  elevator_cpy.position; i <=  maximum; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0)))////////Car and Landing UP
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        for (i =  all_requestsDOWN[floor_index].request.position; i <=  maximum; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if (elevator_cpy.direction == DOWN)
      {
        TravelingTime = (elevator_cpy.position - all_requestsDOWN[floor_index].request.position) * FloorTime;
        for (i =  all_requestsDOWN[floor_index].request.position;i<elevator_cpy.position;i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
      {
        TravelingTime =(elevator_cpy.position - (all_requestsDOWN[floor_index].request.position)) * FloorTime;
        for (i =  all_requestsDOWN[floor_index].request.position; i <=  maximum; i++)
        {
          if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
          {
            elevator_cpy.internal_request[i].active = 0;        //clear Car Call
            RegCall++;
          }
        }
        TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
      }
      break;
    }  //switch(SwitchMode)
    break;
  }   //switch (all_requests[floor_index].request.direction)
  return (TravelingTime + TotalStopageTime);
}
int CalcAverageWaitingTime(int ElevatorCounter, int floor_index) //calculat the average waiting time 
{
  Elevator_TypeDef elevator_cpy;
  elevator_ctrl_get_elevator_status((Elevator_TypeDef*)&elevator_cpy, (Elevator_No_TypeDef)ElevatorCounter);
  int SwitchMode = 0;
  int RegCall = 0;
  int i = 0;
  int TravelingTime = 0;                       //traveling time
  int TotalStopageTime = 0;                    //Stop Time
  int minimum;
  int maximum;
  for (int c = NUMBER_FLOOR-1; c >= 0; c--)
  {
    if ((elevator_cpy.external_requestUP[c].active != 0) || (elevator_cpy.external_requestDOWN[c].active != 0) || (elevator_cpy.internal_request[c].active != 0))//minimum and maximum
    {
      maximum = (Position_Enm_TypeDef)c;
      break;
    }
  }
  for (int c = 0; c <= (NUMBER_FLOOR-1); c++)
  {
    if ((elevator_cpy.external_requestUP[c].active != 0) || (elevator_cpy.external_requestDOWN[c].active != 0) || (elevator_cpy.internal_request[c].active != 0))//minimum and maximum
    {
      minimum = (Position_Enm_TypeDef)c;
      break;
    }
  }
  //  for (int floor = (NUMBER_FLOOR-1); floor >= 0; floor--)
  //  {
  //    if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
  //    {
  //      maximum = floor;
  //      break;
  //    }
  //  }
  //  for (int floor = 0; floor <=(NUMBER_FLOOR-1); floor++)
  //  {
  //    if ((elevator_cpy.external_requestUP[floor].active ==1) || (elevator_cpy.external_requestDOWN[floor].active ==1) || (elevator_cpy.internal_request[floor].active ==1))//minimum and maximum
  //    {
  //      minimum = floor;
  //      break;
  //    }
  //  }                         /*! Car is above Request */
  switch (all_requestsUP[floor_index].request.direction)
  {
  case UP:
    {
      if ((all_requestsUP[floor_index].request.position > elevator_cpy.position))
  {
    SwitchMode = 1;      /*! Car position is < Request Position*/
  }                            /*! Car is under Request */
  else if ((all_requestsUP[floor_index].request.position < elevator_cpy.position))
  {
    SwitchMode = 2;       /*! Car position is > Request Position*/
  }
      switch (SwitchMode)
      {
      case 1:   /*! Car position is < Request Position*/
        {
          RegCall = 0;
          if (elevator_cpy.direction == UP)
          {
            TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
            for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || (elevator_cpy.external_requestUP[i].active != 0))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;           //clear Car Call
                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          else if (elevator_cpy.direction == DOWN)
          {
            TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
            for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || (elevator_cpy.external_requestUP[i].active != 0))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;           //clear Car Call
                RegCall++;
              }
            }
            for (i = minimum; i <= elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;                            //clear Car Call
                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
          {
            TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
            for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;           //clear Car Call

                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          break;
        }
        /*! Car position is > Request Position*/
      case 2:
        {
          RegCall = 0;
          if ((elevator_cpy.direction == UP))
          {
            if (minimum > all_requestsUP[floor_index].request.position)
            {
              TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsUP[floor_index].request.position)) * FloorTime;
              for (i = elevator_cpy.position; i <= maximum; i++)
              {
                if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
                {
                  elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                  RegCall++;
                }
              }
              for (i = all_requestsUP[floor_index].request.position; i <= maximum; i++)
              {
                if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
                {
                  elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                  RegCall++;
                }
              }
            }
            else
            {
              TravelingTime = ((maximum - elevator_cpy.position) + (all_requestsUP[floor_index].request.position - minimum) + (maximum - minimum)) * FloorTime;
              for (i = elevator_cpy.position; i <= maximum; i++)
              {
                if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
                {
                  elevator_cpy.internal_request[i].active = 0;           //clear Car Call

                  RegCall++;
                }
              }
              for (i = minimum; i <= maximum; i++)
              {
                if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
                {
                  elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                  RegCall++;
                }
              }
              for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
              {
                if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
                {
                  elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                  RegCall++;
                }
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          else if (elevator_cpy.direction == DOWN)
          {
            
            TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
            for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                RegCall++;
              }
            }
            for (i = minimum; i <= elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
            
          }
          else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
          {
            
            TravelingTime = (elevator_cpy.position - all_requestsUP[floor_index].request.position) * FloorTime;
            for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call

                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
            
          }
          break;
        }
        break;
      }
    }
  case DOWN:
    {
      if ((external_requestDOWN[floor_index].request.position > elevator_cpy.position))
  {
    SwitchMode = 1;      /*! Car position is < Request Position*/
  }                            /*! Car is under Request */
  else if ((external_requestDOWN[floor_index].request.position < elevator_cpy.position))
  {
    SwitchMode = 2;       /*! Car position is > Request Position*/
  }
    switch (SwitchMode)
    {
      /*! Car position is < Request Position*/
    case 1:
      {
        if(maximum<all_requestsUP[floor_index].request.position) maximum = all_requestsUP[floor_index].request.position;
        RegCall = 0;
        if (elevator_cpy.direction == UP)
        {
          TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsUP[floor_index].request.position)) * FloorTime;
          for (i = elevator_cpy.position; i <= maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;           //clear Car Call

              RegCall++;
            }
          }
          for (i = all_requestsUP[floor_index].request.position; i <= maximum; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
             
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if (elevator_cpy.direction == DOWN)
        {
          if (all_requestsUP[floor_index].request.position >= maximum)
          {
            TravelingTime = ((elevator_cpy.position - minimum) + (all_requestsUP[floor_index].request.position - minimum)) * FloorTime;
            for (i = minimum; i <= elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
               
                RegCall++;
              }
            }
            for (i = minimum; i < all_requestsUP[floor_index].request.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              
                RegCall++;
              }
            }
          }
          else
          {
            //							
            TravelingTime = ((elevator_cpy.position - minimum) + (maximum - minimum) + (maximum - all_requestsUP[floor_index].request.position)) * FloorTime;
            for (i = minimum; i <= elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              
                RegCall++;
              }
            }
            for (i = minimum; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
               
                RegCall++;
              }
            }
            for (i = all_requestsUP[floor_index].request.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
              
                RegCall++;
              }
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
        {
          TravelingTime = (all_requestsUP[floor_index].request.position - elevator_cpy.position) * FloorTime;
          for (i = elevator_cpy.position; i < all_requestsUP[floor_index].request.position; i++)
          {
            if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
            {
              elevator_cpy.internal_request[i].active = 0;        //clear Car Call
             
              RegCall++;
            }
          }
          TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
        }
        break;
      }
      /*! Car position is > Request Position*/
    case 2:
        {
          RegCall = 0;
          if (elevator_cpy.direction == UP)
          {
            TravelingTime = ((maximum - elevator_cpy.position) + (maximum - all_requestsUP[floor_index].request.position)) * FloorTime;
            for (i = elevator_cpy.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestUP[i].active != 0) ))////////Car and Landing UP
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
               
                RegCall++;
              }
            }
            for (i = all_requestsUP[floor_index].request.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
               
                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          
          else if (elevator_cpy.direction == DOWN)
          {
            
            TravelingTime = (elevator_cpy.position - all_requestsUP[floor_index].request.position) * FloorTime;
            for (i = all_requestsUP[floor_index].request.position; i < elevator_cpy.position; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[ i].active != 0) ))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
                
                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          else if ((elevator_cpy.direction == STOP) && (elevator_cpy.Speed == NOT_MOVE))
          {
            
            TravelingTime = (elevator_cpy.position - (all_requestsUP[floor_index].request.position)) * FloorTime;
            for (i = all_requestsUP[floor_index].request.position; i <= maximum; i++)
            {
              if ((elevator_cpy.internal_request[i].active != 0) || ((elevator_cpy.external_requestDOWN[i].active != 0)))////////Car and Landing DOWN
              {
                elevator_cpy.internal_request[i].active = 0;        //clear Car Call
      
                RegCall++;
              }
            }
            TotalStopageTime = (RegCall * elevator_cpy.StoppageTime);
          }
          break;
        }
      }  //switch(SwitchMode)
      break;
    }
  }   //switch (all_requestsUP[floor_index].request.direction)
  return (TravelingTime + TotalStopageTime);
}