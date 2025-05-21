const { Client, GatewayIntentBits, Events } = require('discord.js');
const { joinVoiceChannel, entersState, VoiceConnectionStatus } = require('@discordjs/voice');

const client = new Client({ intents: [GatewayIntentBits.Guilds, GatewayIntentBits.GuildVoiceStates] });

const TOKEN = 'SEU_TOKEN_DISCORD_BOT';

client.once(Events.ClientReady, () => {
  console.log(`Bot online: ${client.user.tag}`);
});

client.on(Events.InteractionCreate, async interaction => {
  if (!interaction.isCommand()) return;

  if (interaction.commandName === 'entrarchamada') {
    const channel = interaction.member.voice.channel;
    if (!channel) return interaction.reply("Entre em um canal de voz primeiro!");

    const connection = joinVoiceChannel({
      channelId: channel.id,
      guildId: channel.guild.id,
      adapterCreator: channel.guild.voiceAdapterCreator,
    });

    await entersState(connection, VoiceConnectionStatus.Ready, 30_000);
    interaction.reply("Entrei na chamada!");

    // O OBS envia o vídeo como "webcam virtual"
  }
});

client.login(TOKEN);
