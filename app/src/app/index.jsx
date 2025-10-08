import useBle from '@/hooks/useBle';
import { useRouter } from 'expo-router';
import { Button, StyleSheet, Text, View } from 'react-native';
import Logo from '../components/logo';


const Home = () => {

  const router = useRouter();

  const ble = useBle();

  const connectPress = () => {
    console.log("Connect pressed")
    router.navigate('/controls')
  }

  return (
    <View style={styles.container}>
  
      <Logo></Logo>
            
      <View>
            <Text style={styles.textStyle}>Connect to the controller</Text>
            <Button onPress={connectPress} title="Connect"/>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        backgroundColor : '#000'
  },

  textStyle : {
    padding : 15,
    color : '#FFF'
  },
});

export default Home;